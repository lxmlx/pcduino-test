int raise (int signum)
{
	/* Even if printf() is available, it's large. Punt it for SPL builds */
	return 0;
}
