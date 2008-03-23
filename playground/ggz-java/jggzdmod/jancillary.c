#include <stdio.h>
#include <jni.h>
#include <ggz.h>
#include <sys/types.h>
#include <unistd.h>

jint Java_GGZDMod_GGZChannelPoller_polldescriptors(JNIEnv *env, jobject obj, jintArray fds)
{
	int ret, i;
	int max;
	fd_set set;

	jint *ints = (*env)->GetIntArrayElements(env, fds, NULL);
	jsize size = (*env)->GetArrayLength(env, fds);

	FD_ZERO(&set);
	max = 0;
	for(i = 0; i < size; i++)
	{
		FD_SET(ints[i], &set);
		if(ints[i] > max)
			max = ints[i];
	}

	ret = select(max + 1, &set, NULL, NULL, NULL);

	if(ret != -1)
	{
		ret = -1;
		for(i = 0; i < size; i++)
			if(FD_ISSET(ints[i], &set))
				ret = ints[i];
	}

	(*env)->ReleaseIntArrayElements(env, fds, ints, JNI_ABORT);

	return ret;
}

jint Java_GGZDMod_GGZChannel_readbuffer(JNIEnv *env, jobject obj, jbyteArray dst, jint length)
{
	int ret;

	jclass jclass = (*env)->GetObjectClass(env, obj);
	jfieldID jfid = (*env)->GetFieldID(env, jclass, "fd", "I");
	jint fd = (*env)->GetIntField(env, obj, jfid);

	jbyte *bytes = (*env)->GetByteArrayElements(env, dst, NULL);

	ret = ggz_readn(fd, bytes, length);

	(*env)->ReleaseByteArrayElements(env, dst, bytes, JNI_COMMIT);

	return ret;
}

jint Java_GGZDMod_GGZChannel_writebuffer(JNIEnv *env, jobject obj, jbyteArray src, jint length)
{
	int ret;

	jclass jclass = (*env)->GetObjectClass(env, obj);
	jfieldID jfid = (*env)->GetFieldID(env, jclass, "fd", "I");
	jint fd = (*env)->GetIntField(env, obj, jfid);

	jbyte *bytes = (*env)->GetByteArrayElements(env, src, NULL);

	ret = ggz_writen(fd, bytes, length);

	(*env)->ReleaseByteArrayElements(env, src, bytes, JNI_ABORT);

	return ret;
}

jint Java_GGZDMod_GGZChannel_readfd(JNIEnv *env, jobject obj)
{
	int xfd, ret;

	jclass jclass = (*env)->GetObjectClass(env, obj);
	jfieldID jfid = (*env)->GetFieldID(env, jclass, "fd", "I");
	jint fd = (*env)->GetIntField(env, obj, jfid);

	ret = ggz_read_fd(fd, &xfd);
	if(ret == -1)
		xfd = -1;

	return xfd;
}

void Java_GGZDMod_GGZChannel_sendfd(JNIEnv *env, jobject obj, jint xfd)
{
	jclass jclass = (*env)->GetObjectClass(env, obj);
	jfieldID jfid = (*env)->GetFieldID(env, jclass, "fd", "I");
	jint fd = (*env)->GetIntField(env, obj, jfid);

	(void)ggz_write_fd(fd, xfd);
}
