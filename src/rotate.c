/**********************************************************************/
/*   Function  to  rotate a block of memory. Can be used for example  */
/*   with an array of pointers.					      */
/**********************************************************************/
/*# include	<malloc.h>*/
# include	<memory.h>
# include	<stdlib.h>

# if defined(MSDOS) || defined(__MSDOS__)
# 	define	TMP_SIZE	1024
# else
# 	define	TMP_SIZE	8192
# endif

static void	my_memcpy(char *p1, char *p2, int len);

/**********************************************************************/
/*   If amount > 0 then copy from start=>start+amount.		      */
/*   If amount < 0 then copy from start+amount=>start.		      */
/**********************************************************************/
void
rotate_mem(char *ptr, char *ptr_end, int amount)
{	char	tmpbuf[TMP_SIZE];
	char	*tmp;
	int	abs_amount = amount < 0 ? -amount : amount;
	int	size = ptr_end - ptr;

	if (amount == 0 || size == 0)
		return;

	if (abs_amount <= TMP_SIZE)
		tmp = tmpbuf;
	else
		tmp = malloc(abs_amount);
	if (amount < 0) {
		memcpy(tmp, ptr, abs_amount);
		memcpy(ptr, ptr + abs_amount, size - abs_amount);
		memcpy(ptr + size - abs_amount, tmp, abs_amount);
		}
	else {
		memcpy(tmp, ptr + size - abs_amount, abs_amount);
		my_memcpy(ptr + abs_amount, ptr, size - abs_amount);
		memcpy(ptr, tmp, abs_amount);
		}
	if (tmp != tmpbuf)
		free(tmp);
}
static void
my_memcpy(char *p1, char *p2, int len)
{
	p1 += len;
	p2 += len;
	if (((int) (long) p1 & 3) == 0 && ((int) (long) p2 & 3) == 0 && (len & 3) == 0
	   && sizeof(long) == 4) {
		while (len >= (int) sizeof(long)) {
			p1 -= (int) sizeof(long);
			p2 -= (int) sizeof(long);
			*(long *) p1 = *(long *) p2;
			len -= sizeof(long);
			}
		}
	while (len-- > 0)
		*--p1 = *--p2;
}
# if TEST
char	*test1 = "abcdefghijklmnopq";
int	test2[] = {
	1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 0
	};

int
main(int argc, char **argv)
{	int	n = atoi(argv[1]);
	int	i;
	rotate_mem(test1, test1 + strlen(test1), n);
	printf("Test1: %s\n", test1);
	rotate_mem(test2, &test2[10], n * sizeof(int));
	printf("Test2: ");
	for (i = 0; i < 11; i++)
		printf(" %d ", test2[i]);
	printf("\n");
}
# endif

