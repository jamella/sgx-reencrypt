#include "policy.h"

static reencrypt_status authorized_clid(const struct keydata_t *k,
                                        const client_id clid) {
    uint32_t i;
    for (i = 0; i < k->n_authorized_clients; ++i) {
        // break if clid is found
        if (!memcmp(clid, k->authorized_clients[i], sizeof(client_id))) {
			return REENCRYPT_OK;
        }
    }
    return REENCRYPT_FAILED;
}

reencrypt_status authorized_from(const struct keydata_t *k,
                                 const key_id keyIDin) {
    uint32_t i;
    if (k->policy_from == POLICY_ALL) {
        return REENCRYPT_OK;
    }
    for (i = 0; i < k->n_keys_from; ++i) {
        // break if keyid is found
        if (!memcmp(keyIDin, k->keys_from[i], sizeof(key_id))) {
            return REENCRYPT_OK;
        }
    }
    return REENCRYPT_FAILED;
}

reencrypt_status authorized_to(const struct keydata_t *k,
                               const key_id keyIDout) {
    uint32_t i;
    if (k->policy_to == POLICY_ALL) {
        return REENCRYPT_OK;
    }
    for (i = 0; i < k->n_keys_to; ++i) {
        // break if keyid is found
        if (!memcmp(keyIDout, k->keys_to[i], sizeof(key_id))) {
            return REENCRYPT_OK;
        }
    }
    return REENCRYPT_FAILED;
}

reencrypt_status check_policy(struct keydata_t **keyin, struct keydata_t **keyout,
                              const client_id clid, const key_id keyIDin,
                              const key_id keyIDout) {
    reencrypt_status ret = REENCRYPT_FAILED;
    if ((get_key(keyIDin, keyin) == REENCRYPT_FAILED) ||
        (get_key(keyIDout, keyout) == REENCRYPT_FAILED)) {
        return REENCRYPT_KEY_NOTFOUND;
    }

    // check if clid is accepted on both keys
    if (authorized_clid(*keyin, clid) != REENCRYPT_OK ||
        authorized_clid(*keyout, clid) != REENCRYPT_OK) {
        return REENCRYPT_NOT_AUTHORIZED;
    }

    // is keyIDout amongst keyin.keys_to and keyIDin amongst keyout.keys_from?
    if (authorized_to(*keyin, keyIDout) != REENCRYPT_OK ||
        authorized_from(*keyout, keyIDin) != REENCRYPT_OK) {
        return REENCRYPT_NOT_AUTHORIZED;
    }

    // life is good
    return REENCRYPT_OK;
}
