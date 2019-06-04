#ifndef FLAG_H
#define FLAG_H

/* 32个flag有效 */
static int __ella_flag_access(unsigned int bit, unsigned char val, unsigned char _set)
{
	static unsigned int s_flags = 0;
	if(_set) {
		if(val) {
			s_flags |= bit;
		} else {
			s_flags &= ~bit;
		}
	}

	return !!(s_flags & bit);
}

int ella_flag_set(unsigned int bit)
{
	return __ella_flag_access(bit, true, true);
}

int ella_flag_set_ex(unsigned int bit, unsigned char val)
{
	return __ella_flag_access(bit, val, true);
}

int ella_flag_get(unsigned int bit)
{
	return __ella_flag_access(bit, false, false);
}

int ella_flag_clr(unsigned bit)
{
	return __ella_flag_access(bit, false, true);
}
#endif /* FLAG_H */