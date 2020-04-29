- Each interrupt controller is given a domain, which functions like the address space for a
process. It manages mappings between hardware IRQs and Linux IRQs (that is, virtual IRQs).
It is the hardware interrupt number translation object.

- An interrupt controller driver creates and registers an irq_domain by calling one of the
irq_domain_add_<mapping_method>() functions, where <mapping_method> is the
method by which hwirq should be mapped to the Linux IRQ. These are:
	+ irq_domain_add_linear(): This uses a fixed-size table, indexed by the hwirq
number. 
	+ When an hwirq is mapped, an IRQ_DESC is allocated for the hwirq, and
the IRQ number is stored in the table. 

- Since the IRQ domain was empty at creation time (no mapping), you should use the
irq_create_mapping() function to create mapping and assign it to the domain. 

- When writing drivers for GPIO controllers that are also interrupt controllers,
irq_create_mapping() is called from within the gpio_chip.to_irq() callback
function.

- If a mapping for the hwirq doesn't already exist, the function will allocate a new Linux
irq_desc structure, associate it with the hwirq, and call the irq_domain_ops.map()
callback.
- .map(): This creates or updates a mapping between a virtual irq (virq) number
and an hwirq number. This is called only once for a given mapping. It generally
maps the virq with a given handler, using irq_set_chip_and_handler*, so
that calling generic_handle_irq()or handle_nested_irq will trigger the
right handler.



/* struct irq_chip - hardware interrupt chip descriptor */
struct irq_chip {
	void (*call_back)(struct irq_data *data);
}
/* struct irq_domain - Hardware interrupt number translation object */
struct irq_domain {

	const struct irq_domain_ops *ops;

	unsigned int mapcount;
	struct irq_domain_chip_generic *gc;

	/* reverse map data. The linear map gets appended to the irq_domain */
	irq_hw_number_t hwirq_max;
	unsigned int revmap_direct_max_irq;
	unsigned int revmap_size;
	struct radix_tree_root revmap_tree;
	unsigned int linear_revmap[];
};

/* struct irq_data - per irq chip data passed down to chip functions */
struct irq_data {
	unsigned int		irq;
	unsigned long		hwirq;
	struct irq_chip		*chip;
	struct irq_domain	*domain;
	void			*chip_data;
};
/* struct irq_desc - interrupt descriptor */
struct irq_desc {
	irq_flow_handler_t	handle_irq;

	struct irqaction	*action;	/* IRQ action list */
	unsigned int		irq_count;	/* For detecting broken IRQs */
	atomic_t		threads_handled;
	int			threads_handled_last;

	int			parent_irq;
	struct module		*owner;
	const char		*name;
}

int irq_domain_associate(struct irq_domain *domain, unsigned int virq,
			 irq_hw_number_t hwirq)
{
	struct irq_data *irq_data = irq_get_irq_data(virq);
	int ret;

	if (domain->ops->map) {
		ret = domain->ops->map(domain, virq, hwirq); /* binding irq and handler */
	}

	irq_domain_set_mapping(domain, hwirq, irq_data);

	return 0;
}

/* domain->ops->map(domain, virq, hwirq); */
static void irq_domain_set_mapping(struct irq_domain *domain,
				   irq_hw_number_t hwirq,
				   struct irq_data *irq_data)
{
	if (hwirq < domain->revmap_size) {
		domain->linear_revmap[hwirq] = irq_data->irq;
	} else {
		mutex_lock(&revmap_trees_mutex);
		radix_tree_insert(&domain->revmap_tree, hwirq, irq_data);
		mutex_unlock(&revmap_trees_mutex);
	}
}