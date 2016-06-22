#ifndef SYNTHESIZER_H_INCLUDED
#define SYNTHESIZER_H_INCLUDED

/*
 * <synthesizer> decl
 */
struct synthesizer {
        float   freq[88];
};

/*
 * <synthesizer> public
 */
void syn_init(struct synthesizer* self);
void syn_free(struct synthesizer* self);


#endif // SYNTHESIZER_H_INCLUDED
