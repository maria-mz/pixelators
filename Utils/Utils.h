#ifndef UTILS_H_
#define UTILS_H_

inline int computeHP(int health, int maxHealth)
{
    return health * 100.0f / maxHealth;
}

#endif