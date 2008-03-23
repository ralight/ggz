#include <stdio.h>
#include <jni.h>
#include <ggz.h>

jint Java_GGZChannel_readbuffer(JNIEnv *env, jobject obj, jbyteArray dst, jint length)
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

jint Java_GGZChannel_writebuffer(JNIEnv *env, jobject obj, jbyteArray src, jint length)
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

jint Java_GGZChannel_readfd(JNIEnv *env, jobject obj)
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

void Java_GGZChannel_sendfd(JNIEnv *env, jobject obj, jint xfd)
{
	jclass jclass = (*env)->GetObjectClass(env, obj);
	jfieldID jfid = (*env)->GetFieldID(env, jclass, "fd", "I");
	jint fd = (*env)->GetIntField(env, obj, jfid);

	(void)ggz_write_fd(fd, xfd);
}
