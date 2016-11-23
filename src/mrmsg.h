/*******************************************************************************
 *
 *                             Messenger Backend
 *     Copyright (C) 2016 Björn Petersen Software Design and Development
 *                   Contact: r10s@b44t.com, http://b44t.com
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see http://www.gnu.org/licenses/ .
 *
 *******************************************************************************
 *
 * File:    mrmsg.h
 * Authors: Björn Petersen
 * Purpose: mrmsg_t represents a single message in a chat.  One E-Mail can
 *          result in different messages!
 *
 ******************************************************************************/


#ifndef __MRMSG_H__
#define __MRMSG_H__
#ifdef __cplusplus
extern "C" {
#endif


#include "mrparam.h"
typedef struct mrjob_t mrjob_t;


/* message types */
#define MR_MSG_UNDEFINED   0
#define MR_MSG_TEXT        10
#define MR_MSG_IMAGE       20 /* param: 'f'ile, 'w', 'h' */
#define MR_MSG_AUDIO       40 /* param: 'f'ile, 'd'uration */
#define MR_MSG_VIDEO       50 /* param: 'f'ile, 'w', 'h', 'd'uration */
#define MR_MSG_FILE        60 /* param: 'f'ile */


/* message states */
#define MR_STATE_UNDEFINED  0
#define MR_IN_UNSEEN       10 /* incoming message not read */
#define MR_IN_SEEN         16 /* incoming message read */
#define MR_OUT_PENDING     20 /* hit "send" button - but the message is pending in some way, maybe we're offline (no checkmark) */
#define MR_OUT_SENDING     22 /* the message is just now being sending */
#define MR_OUT_ERROR       24 /* unrecoverable error (recoverable errors result in pending messages) */
#define MR_OUT_DELIVERED   26 /* outgoing message successfully delivered to server (one checkmark) */
#define MR_OUT_READ        28 /* outgoing message read (two checkmarks; this requires goodwill on the receiver's side) */


typedef struct mrmsg_t
{
	uint32_t      m_magic;
	uint32_t      m_id;
	char*         m_rfc724_mid;
	char*         m_server_folder;
	uint32_t      m_server_uid;
	uint32_t      m_from_id;   /* contact, 0=unset, 1=self .. >9=real contacts */
	uint32_t      m_to_id;     /* contact, 0=unset, 1=self .. >9=real contacts */
	uint32_t      m_chat_id;   /* the chat, the message belongs to: 0=unset, 1=unknwon sender .. >9=real chats */
	time_t        m_timestamp; /* unix time the message was sended */

	int           m_type;      /* MR_MSG_* */
	int           m_state;     /* MR_STATE_* etc. */
	int           m_is_msgrmsg;
	char*         m_text;      /* message text or NULL if unset */
	mrparam_t*    m_param;     /* 'f'ile, 'm'ime, 'w', 'h', 'd'uration/ms etc. depends on the type, != NULL */
	int           m_bytes;     /* used for external BLOBs, BLOB data itself is stored in plain files with <8-chars-hex-id>.ext, 0 for plain text */
} mrmsg_t;


mrmsg_t*     mrmsg_new                    ();
void         mrmsg_unref                  (mrmsg_t*); /* this also free()s all strings; so if you set up the object yourself, make sure to use strdup()! */
void         mrmsg_empty                  (mrmsg_t*);


/*** library-private **********************************************************/

#define      MR_MSG_FIELDS                    " m.id,rfc724_mid,m.server_folder,m.server_uid,m.chat_id, m.from_id,m.to_id,m.timestamp, m.type,m.state,m.msgrmsg,m.txt, m.param,m.bytes "
int          mrmsg_set_from_stmt__            (mrmsg_t*, sqlite3_stmt* row, int row_offset); /* row order is MR_MSG_FIELDS */
int          mrmsg_load_from_db__             (mrmsg_t*, mrsqlite3_t*, uint32_t id);
char*        mrmsg_get_summary                (int type, const char* text, int approx_bytes); /* the returned values must be free()'d */
size_t       mrmailbox_get_real_msg_cnt__     (mrmailbox_t*); /* the number of messages assigned to real chat (!=deaddrop, !=trash) */
size_t       mrmailbox_get_deaddrop_msg_cnt__ (mrmailbox_t*);
int          mrmailbox_rfc724_mid_cnt__       (mrmailbox_t*, const char* rfc724_mid);
int          mrmailbox_rfc724_mid_exists__    (mrmailbox_t*, const char* rfc724_mid, char** ret_server_folder, uint32_t* ret_server_uid);
void         mrmailbox_update_server_uid__    (mrmailbox_t*, const char* rfc724_mid, const char* server_folder, uint32_t server_uid);
void         mrmailbox_update_msg_chat_id__   (mrmailbox_t*, uint32_t msg_id, uint32_t chat_id);
void         mrmailbox_update_msg_state__     (mrmailbox_t*, uint32_t msg_id, int state);
void         mrmailbox_delete_msg_on_imap     (mrmailbox_t* mailbox, mrjob_t* job);
void         mrmailbox_markseen_msg_on_imap   (mrmailbox_t* mailbox, mrjob_t* job);

#define      MR_GHOST_ID_FORMAT               "G@%lu"

#ifdef __cplusplus
} /* /extern "C" */
#endif
#endif /* __MRMSG_H__ */

