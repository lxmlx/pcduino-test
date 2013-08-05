#ifndef _AXP209_H
#define _AXP209_H

int axp209_set_dcdc2(int mvolt);
int axp209_set_dcdc3(int mvolt);
int axp209_set_ldo2(int mvolt);
int axp209_set_ldo3(int mvolt);
int axp209_set_ldo4(int mvolt);
void axp209_poweroff(void);
int axp209_init(void);

#endif
