
/*********************************************************************
 *
 * Copyright (C) 2020 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without 
 * the express written permission of the copyright holder.
 *
 ***********************************************************************/

#include "manpage.h"
#include <pthread.h>

pthread_mutex_t locks[7];
int k;

void* thread_func(void* args) {
	int p_id = getParagraphId();
	pthread_mutex_lock(&locks[p_id]);
	showParagraph();
	pthread_mutex_unlock(&locks[p_id]);
	k++;
	if(k >= 7)  return NULL;
	pthread_mutex_unlock(&locks[k]);
	return NULL;
}

/*
 * See manpage.h for details.
 *
 * As supplied, shows random single messages.
 */
void manpage()
{
	k = 0;
	for(int i = 0; i < 7; i++) {
		pthread_mutex_lock(&locks[i]);
	}
	pthread_t threads[7];
	for(int i = 0; i < 7; i++) {
		pthread_create(&threads[i], NULL, thread_func, NULL);
	}
	pthread_mutex_unlock(&locks[k]);
	for(int i = 0; i < 7; i++) {
		pthread_join(threads[i], NULL);
	}
}
