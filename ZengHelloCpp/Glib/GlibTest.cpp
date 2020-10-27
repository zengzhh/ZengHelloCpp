#include "GlibTest.h"
#include <iostream>
#include <glib.h>
#include <glib/gprintf.h>

using namespace std;

void GlibTest::test_StringUtility()
{
	cout << "**** test_StringUtility()" << endl;
	gchar* str = g_strdup_printf("g_strdup_printf %d", 0);
	printf("%s\n", str);
	g_free(str);
}

void GlibTest::test_Base64Encoding()
{
	cout << "**** test_Base64Encoding()" << endl;
	const guchar* data = (const guchar*)g_strdup("test");
	gchar* base64 = g_base64_encode(data, 4);
	printf("%s\n", base64);
}

