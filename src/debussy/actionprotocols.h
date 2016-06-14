#ifndef ACTIONPROTOCOLS_H_INCLUDED
#define ACTIONPROTOCOLS_H_INCLUDED

#include <types.h>

enum ActionType {
        ActionPutString,
};

/*
 * <action_protocol> decl
 */
struct action_protocol {
        uint8_t action_type;
};


#endif // ACTIONPROTOCOLS_H_INCLUDED
