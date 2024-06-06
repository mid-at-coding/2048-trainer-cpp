#ifndef UI_H
#define UI_H
#include <optional>
#include <string>
std::optional<int> getInt();
std::optional<bool> getBool();
void setBool(bool *ret);
void setInt(int *ret);
int getNumberInRange(unsigned floor, unsigned ceil, std::string valueName);
#endif
