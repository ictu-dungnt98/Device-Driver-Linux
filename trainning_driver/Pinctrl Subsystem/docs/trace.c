static int pcs_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;

	pcs->dev = &pdev->dev;
	pcs->np = np;

	ret = of_property_read_u32(np, "pinctrl-single,register-width", &pcs->width);

	ret = of_property_read_u32(np, "pinctrl-single,function-mask", &pcs->fmask);
	if (!ret) {
		pcs->fshift = __ffs(pcs->fmask);
		pcs->fmax = pcs->fmask >> pcs->fshift;
	}

	ret = pcs_quirk_missing_pinctrl_cells(pcs, np, pcs->bits_per_mux ? 2 : 1);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	pcs->size = resource_size(pcs->res);

	platform_set_drvdata(pdev, pcs);

	pcs->read = pcs_readl;
	pcs->write = pcs_writel;

	pcs->desc.name = DRIVER_NAME;
	pcs->desc.pctlops = &pcs_pinctrl_ops;
	pcs->desc.pmxops = &pcs_pinmux_ops;
	pcs->desc.confops = &pcs_pinconf_ops;
	pcs->desc.owner = THIS_MODULE;

	ret = pcs_allocate_pin_table(pcs);

	ret = pinctrl_register_and_init(&pcs->desc, pcs->dev, pcs, &pcs->pctl);

	ret = pcs_add_gpio_func(np, pcs);
	/* pcs->pctl->dev = pcs->dev = &pdev->dev */
	return pinctrl_enable(pcs->pctl);

}

int pinctrl_enable(struct pinctrl_dev *pctldev)
{
	int error;

	error = pinctrl_claim_hogs(pctldev);

	return 0;
}

static int pinctrl_claim_hogs(struct pinctrl_dev *pctldev)
{
	pctldev->p = create_pinctrl(pctldev->dev, pctldev);

	pctldev->hog_default = pinctrl_lookup_state(pctldev->p, PINCTRL_STATE_DEFAULT);

	if (pinctrl_select_state(pctldev->p, pctldev->hog_default))
		dev_err(pctldev->dev, "failed to select default state\n");

	return 0;
}

static struct pinctrl *create_pinctrl(struct device *dev,
				      struct pinctrl_dev *pctldev)
{
	struct pinctrl *p;
	const char *devname;
	struct pinctrl_maps *maps_node;
	int i;
	const struct pinctrl_map *map;
	int ret;

	/*
	 * create the state cookie holder struct pinctrl for each
	 * mapping, this is what consumers will get when requesting
	 * a pin control handle with pinctrl_get()
	 */
	p = kzalloc(sizeof(*p), GFP_KERNEL);

	p->dev = dev;

	ret = pinctrl_dt_to_map(p, pctldev);

	devname = dev_name(dev);

	/* Iterate over the pin control maps to locate the right ones */
	for_each_maps(maps_node, i, map) {
		/* Map must be for this device */
		if (strcmp(map->dev_name, devname))
			continue;
		/*
		 * If pctldev is not null, we are claiming hog for it,
		 * that means, setting that is served by pctldev by itself.
		 *
		 * Thus we must skip map that is for this device but is served
		 * by other device.
		 */
		if (pctldev && strcmp(dev_name(pctldev->dev), map->ctrl_dev_name))
			continue;

		ret = add_setting(p, pctldev, map);
	}

	return p;
}

int pinctrl_dt_to_map(struct pinctrl *p, struct pinctrl_dev *pctldev)
{
	struct device_node *np = p->dev->of_node;
	int state, ret;
	char *propname;
	struct property *prop;
	const char *statename;
	const __be32 *list;
	int size, config;
	phandle phandle;
	struct device_node *np_config;


	/* We may store pointers to property names within the node */
	of_node_get(np);

	/* For each defined state ID */
	for (state = 0; ; state++) {
		/* Retrieve the pinctrl-* property */
		propname = kasprintf(GFP_KERNEL, "pinctrl-%d", state);
		prop = of_find_property(np, propname, &size);
		kfree(propname);

		list = prop->value;
		size /= sizeof(*list);

		/* Determine whether pinctrl-names property names the state */
		ret = of_property_read_string_index(np, "pinctrl-names",
						    state, &statename);

		/* For every referenced pin configuration node in it */
		for (config = 0; config < size; config++) {
			phandle = be32_to_cpup(list++);

			/* Look up the pin configuration node */
			np_config = of_find_node_by_phandle(phandle);			

			/* Parse the node */
			ret = dt_to_map_one_config(p, pctldev, statename, np_config);
			of_node_put(np_config);
			if (ret < 0)
				goto err;
		}
	}

	return 0;
}

static int dt_to_map_one_config(struct pinctrl *p,
				struct pinctrl_dev *hog_pctldev,
				const char *statename,
				struct device_node *np_config)
{
	struct pinctrl_dev *pctldev = NULL;
	struct device_node *np_pctldev;
	const struct pinctrl_ops *ops;
	int ret;
	struct pinctrl_map *map;
	unsigned num_maps;

	/* Find the pin controller containing np_config */
	np_pctldev = of_node_get(np_config);
	for (;;) {
		np_pctldev = of_get_next_parent(np_pctldev);
		if (!np_pctldev || of_node_is_root(np_pctldev)) {
			dev_info(p->dev, "could not find pctldev for node %pOF, deferring probe\n",
				np_config);
			of_node_put(np_pctldev);
			/* OK let's just assume this will appear later then */
			return -EPROBE_DEFER;
		}
		/* If we're creating a hog we can use the passed pctldev */
		if (hog_pctldev && (np_pctldev == p->dev->of_node)) {
			pctldev = hog_pctldev;
			break;
		}
		pctldev = get_pinctrl_dev_from_of_node(np_pctldev);
		if (pctldev)
			break;
		/* Do not defer probing of hogs (circular loop) */
		if (np_pctldev == p->dev->of_node) {
			of_node_put(np_pctldev);
			return -ENODEV;
		}
	}
	of_node_put(np_pctldev);

	/*
	 * Call pinctrl driver to parse device tree node, and
	 * generate mapping table entries
	 */
	ops = pctldev->desc->pctlops;

	ret = ops->dt_node_to_map(pctldev, np_config, &map, &num_maps);

	/* Stash the mapping table chunk away for later use */
	return dt_remember_or_free_map(p, statename, pctldev, map, num_maps);
}