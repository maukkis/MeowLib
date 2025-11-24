#include "../include/permissions.h"
#include "../include/user.h"
#include <cstdint>

bool hasPermission(const User& user, const Permissions permission){
  if(!user.guild || !user.guild->permissions) return false;
  return *user.guild->permissions & static_cast<uint64_t>(permission);
}

bool hasPermission(const uint64_t permissions, const Permissions permission){
  return permissions & static_cast<uint64_t>(permission);
}
