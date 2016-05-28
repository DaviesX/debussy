#ifndef MUSICPLAYER_H_INCLUDED
#define MUSICPLAYER_H_INCLUDED

/*
 * <note_player>
 */
struct note_player {
        float   freq[88];
};

/*
 * <note_player> public
 */
void note_player_init(struct note_player* self);


#endif // MUSICPLAYER_H_INCLUDED
