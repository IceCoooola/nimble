#ifndef NIMBLE_STRUCTS_H
#define NIMBLE_STRUCTS_H

typedef enum {
    NEW_LEDGER,
    APPEND,
    READ_LAST
} RequestType;

typedef struct 
{
    uint32_t cur_state; // use int as current state
    char encrypted_data[256]; // use RSA. σ = Sign(secret_key, (l, S′, c + 1))
} Block;

typedef struct 
{
    char encrypted_data[256]; // something like: Sign(sk, ⟨“append”, id, l, tcurr , hcurr⟩)
} Receipt;

typedef struct 
{
    // TODO: change later 
    int x;
} Ledger; // label of a Ledger

typedef struct {
    RequestType request;
    Ledger ledger;
} ClientNewLedgerPayload;

typedef struct {
    RequestType request;
    Ledger ledger;
    uint32_t exp_index;
} ClientAppendPayload;

typedef struct 
{
    RequestType request;
    uint32_t nonce;
} ClientReadLastPayload;

typedef struct 
{
    RequestType request;
    bool ack;
    Receipt receipt;
} EndorserNewLedgerRespond;

typedef struct 
{
    RequestType request;
    bool ack;
    Receipt receipt;
} EndorserAppendRespond;

typedef struct 
{
    RequestType request;
    uint32_t index;
    Block block;
    Receipt receipt;
} EndorserReadLastRespond;

#endif