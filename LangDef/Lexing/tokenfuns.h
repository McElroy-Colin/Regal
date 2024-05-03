# include "tokens.h"

#ifndef TOKENFUNS_H
#define TOKENFUNS_H

void free_var(Vari var);
char* disp_token(Token tok, void* obj);

enum SiphonTokens {
    KeywrdSip = 2,
    PrimSip = KeywrdSip + 1,
    PrimOpSip = PrimSip + 5,
    VarSip = PrimOpSip + 2,
    GroupSip = VarSip + 2,
    NothingSip = GroupSip + 1
};

#endif