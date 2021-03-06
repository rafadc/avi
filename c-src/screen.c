#include <jni.h>
#include <curses.h>

static const int SCREEN_COLORS[] = {
	COLOR_BLACK,
	COLOR_RED,
	COLOR_GREEN,
	COLOR_YELLOW,
	COLOR_BLUE,
	COLOR_MAGENTA,
	COLOR_CYAN,
	COLOR_WHITE
};

const int SCREEN_COLOR_COUNT = sizeof(SCREEN_COLORS)/sizeof(SCREEN_COLORS[0]);

JNIEXPORT void JNICALL
Java_avi_terminal_Screen_nativeStart(JNIEnv *env, jclass k)
{
	int i, j;

	initscr();
	start_color();

	for (i = 0; i < SCREEN_COLOR_COUNT; ++i)
		for (j = 0; j < SCREEN_COLOR_COUNT; ++j)
			init_pair(i*SCREEN_COLOR_COUNT+j, SCREEN_COLORS[i], SCREEN_COLORS[j]);
	
	cbreak();
	noecho();
	keypad(stdscr, 1);
}

JNIEXPORT void JNICALL
Java_avi_terminal_Screen_stop(JNIEnv *env, jclass k)
{
	endwin();
}

static jboolean is_enter_key(jchar character) 
{
	return character == KEY_ENTER || character == '\n';
}

static jstring make_ctrl_key(JNIEnv *env, jchar ch)
{
	jchar ctrl[] = {'<', 'C', '-', ch + 0x40, '>'};
	return (*env)->NewString(env, ctrl, 5);
}

JNIEXPORT jstring JNICALL
Java_avi_terminal_Screen_getKey(JNIEnv *env, jclass k)
{
	jchar character = getch();

	if (is_enter_key(character)) 
		return (*env)->NewStringUTF(env, "<Enter>");
	if (KEY_BACKSPACE == character || 127 == character)
		return (*env)->NewStringUTF(env, "<BS>");
	if (27 == character)
		return (*env)->NewStringUTF(env, "<Esc>");

	if (character >= 0 && character < 0x20)
		return make_ctrl_key(env, character);

	return (*env)->NewString(env, &character, 1);
}

JNIEXPORT void JNICALL
Java_avi_terminal_Screen_refresh(JNIEnv *env, jclass k, jint cursorI, jint cursorJ, jint width, jcharArray charsArray, jbyteArray attrsArray)
{
	chtype ch;
	jint i, j, offset;
	jsize size = (*env)->GetArrayLength(env, charsArray);
	jchar *chars = (*env)->GetCharArrayElements(env, charsArray, NULL);
	jbyte *attrs = (*env)->GetByteArrayElements(env, attrsArray, NULL);

	for (i = 0, offset = 0; offset < size; ++i, offset += width) {
		for (j = 0; j < width; ++j) {
			ch = chars[offset+j] | COLOR_PAIR(attrs[offset+j]);
			mvaddch(i, j, ch);
		}
	}

	move(cursorI, cursorJ);
	refresh();

	(*env)->ReleaseCharArrayElements(env, charsArray, chars, 0);
	(*env)->ReleaseByteArrayElements(env, attrsArray, attrs, 0);
}

JNIEXPORT jintArray JNICALL
Java_avi_terminal_Screen_size(JNIEnv *env, jclass k)
{
	jintArray sizeArray;
	jint *size;

	sizeArray = (*env)->NewIntArray(env, 2);
	size = (*env)->GetIntArrayElements(env, sizeArray, NULL);

	getmaxyx(stdscr, size[0], size[1]);

	(*env)->ReleaseIntArrayElements(env, sizeArray, size, 0);
	return sizeArray;
}

JNIEXPORT void JNICALL
Java_avi_terminal_Screen_beep(JNIEnv *env, jclass k)
{
	beep();
}
