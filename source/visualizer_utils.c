#include "malloc.h"

static void				tf_write_int_recusive(uint64_t nbr, uint64_t base_nbr, char *base);
static uint64_t	ft_check_if_valid_base(char *base, uint64_t base_nbr);

void	ft_putnbr_base_u_long(uint64_t nbr, char *base)
{
	uint64_t	base_nbr;

	base_nbr = 0;
	while (base[base_nbr] != '\0')
		base_nbr++;
	if (ft_check_if_valid_base(base, base_nbr))
	{
		if (nbr == 0)
			write(1, &base[0], 1);
		else
			tf_write_int_recusive(nbr, base_nbr, base);
	}
}

static void	tf_write_int_recusive(uint64_t nbr,
						uint64_t base_nbr, char *base)
{
	uint64_t	current_nbr;

	if (nbr != 0)
	{
		current_nbr = (nbr % base_nbr);
		tf_write_int_recusive((nbr - nbr % base_nbr) / base_nbr,
			base_nbr, base);
		write(1, &base[current_nbr], 1);
	}
}

static uint64_t	ft_check_if_valid_base(char *base,
						uint64_t base_nbr)
{
	uint64_t	i;
	uint64_t	j;

	i = 0;
	if (base_nbr < 2)
		return (0);
	while (base[i] != '\0')
	{
		if (base[i] == '-' || base[i] == '+')
			return (0);
		i++;
	}
	i = 0;
	while (base[i] != '\0')
	{
		j = i +1;
		while (base[j] != '\0')
		{
			if (base[j] == base[i])
				return (0);
			j++;
		}
		i++;
	}
	return (1);
}