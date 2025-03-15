#pragma once

#define F_INFINITY (__builtin_inff())
#define F_NAN (__builtin_nanf(""))
#define F_EPSILON_HUMAN 0.000001
#define F_ABS(_f) (_f < 0 ? -_f : _f)
