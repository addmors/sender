// CurlTest.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#define BUILDING_LIBCURL
#define CURL_STATIC
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <iterator>
#include "curl\curl.h"

#ifdef _DEBUG
#pragma comment(lib, "curl/libcurl_a_debug.lib")
#else
#pragma comment(lib, "curl/libcurl_a.lib")
#endif

#include <fstream>

#pragma comment(lib, "Normaliz.lib")
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "Wldap32.lib")
#pragma comment(lib, "advapi32.lib")

/* This is a simple example showing how to send mail using libcurl's SMTP
 * capabilities. It builds on the smtp-mail.c example to add authentication
 * and, more importantly, transport security to protect the authentication
 * details from being snooped.
 *
 * Note that this example requires libcurl 7.20.0 or above.
 */

#define FROM    "<morstas99@mail.ru>"
#define TO      "<morstas99@mail.ru >"
#define CC      "<morstas99@mail.ru>"
static const int CHARS = 76;
static char fileBuf[CHARS*100][CHARS*100];
void buffer_add(std::string st, size_t & buffer, size_t &len) {
	strcpy_s(fileBuf[len++], CHARS, st.c_str());
	buffer += strlen(fileBuf[len - 1]) + 1;
}
std::size_t readFile(const std::string& fileName) {
	setlocale(LC_ALL, "ru");
	std::ifstream f(fileName);
	std::string s;
	size_t buffer_size(0), len(0);
	buffer_add("Date: Mon, 29 Nov 2010 21:54:29 +1100\r\n", buffer_size, len);
	buffer_add("To: " TO "\r\n", buffer_size, len);
	buffer_add("From: " FROM " (Example User)\r\n", buffer_size, len);
	buffer_add("Cc: " CC " (Another example User)\r\n", buffer_size, len);
	buffer_add("Subject: Курсовая работа.\r\n", buffer_size, len);
	buffer_add("Content-Type: multipart/mixed;\r\n", buffer_size, len);
	buffer_add(" boundary=\"------------030203080101020302070708\"\r\n", buffer_size, len);
	buffer_add("--------------030203080101020302070708\r\n", buffer_size, len);
	buffer_add("Content-Type: text/plain; charset=utf-8; format=flowed\r\n", buffer_size, len);
	buffer_add("Content-Transfer-Encoding: 7bit\r\n", buffer_size, len);
	buffer_add("\r\n", buffer_size, len);
	buffer_add("The body of the message starts here.\r\n\r\n", buffer_size, len);
	buffer_add("--------------030203080101020302070708\r\n", buffer_size, len);
	buffer_add("Content-Type: text/plain; charset=utf-8; format=flowed\r\n", buffer_size, len);
	buffer_add("name=\"send.c\"\r\n", buffer_size, len);
	buffer_add("Content-Type: text/plain; charset=utf-8; format=flowed\r\n", buffer_size, len);
	buffer_add("Content-Disposition: attachment;\r\n", buffer_size, len);
	buffer_add("  filename=\"message.txt\"\r\n", buffer_size, len);
	buffer_add("\r\n", buffer_size, len);
	while (std::getline(f, s)) {
		if (s != "") {
			strcpy(fileBuf[len++], s.c_str());
			buffer_size += strlen(fileBuf[len - 1]) + 1;
			buffer_add("\r\n", buffer_size, len);
			buffer_size += strlen(fileBuf[len - 1]) + 1;
		}
	}
	buffer_add("\r\n", buffer_size, len);
	f.close();
	buffer_add("--------------030203080101020302070708--\r\n", buffer_size, len);

	
	return buffer_size;
}

struct upload_status {
	int lines_read;
};

static size_t payload_source(char *ptr, size_t size, size_t nmemb, void *userp)
{
	struct upload_status *upload_ctx = (struct upload_status *)userp;
	const char *data;

	if ((size == 0) || (nmemb == 0) || ((size*nmemb) < 1)) {
		return 0;
	}

	data = fileBuf[upload_ctx->lines_read];

	if (data) {
		size_t len = strlen(data);
		memcpy(ptr, data, len);
		upload_ctx->lines_read++;

		return len;
	}

	return 0;
}

int main(void)
{
	setlocale(LC_ALL, "ru");
	CURL *curl;
	CURLcode res = CURLE_OK;
	struct curl_slist *recipients = NULL;
	struct upload_status upload_ctx;

	upload_ctx.lines_read = 0;
	size_t file_size = readFile("test.txt");
	curl = curl_easy_init();
	if (curl) {
		/* Set username and password */
		curl_easy_setopt(curl, CURLOPT_USERNAME, "morstas99");
		curl_easy_setopt(curl, CURLOPT_PASSWORD, "xxxxxx");

		/* This is the URL for your mailserver. Note the use of port 587 here,
		 * instead of the normal SMTP port (25). Port 587 is commonly used for
		 * secure mail submission (see RFC4403), but you should use whatever
		 * matches your server configuration. */
		curl_easy_setopt(curl, CURLOPT_URL, "smtp://smtp.mail.ru:587");

		/* In this example, we'll start with a plain text connection, and upgrade
		 * to Transport Layer Security (TLS) using the STARTTLS command. Be careful
		 * of using CURLUSESSL_TRY here, because if TLS upgrade fails, the transfer
		 * will continue anyway - see the security discussion in the libcurl
		 * tutorial for more details. */
		curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);

		/* If your server doesn't have a valid certificate, then you can disable
		 * part of the Transport Layer Security protection by setting the
		 * CURLOPT_SSL_VERIFYPEER and CURLOPT_SSL_VERIFYHOST options to 0 (false).
		 *   curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		 *   curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		 * That is, in general, a bad idea. It is still better than sending your
		 * authentication details in plain text though.  Instead, you should get
		 * the issuer certificate (or the host certificate if the certificate is
		 * self-signed) and add it to the set of certificates that are known to
		 * libcurl using CURLOPT_CAINFO and/or CURLOPT_CAPATH. See docs/SSLCERTS
		 * for more information. */
		//curl_easy_setopt(curl, CURLOPT_CAINFO, "google.pem");

		/* Note that this option isn't strictly required, omitting it will result
		 * in libcurl sending the MAIL FROM command with empty sender data. All
		 * autoresponses should have an empty reverse-path, and should be directed
		 * to the address in the reverse-path which triggered them. Otherwise,
		 * they could cause an endless loop. See RFC 5321 Section 4.5.5 for more
		 * details.
		 */
		curl_easy_setopt(curl, CURLOPT_MAIL_FROM, FROM);

		/* Add two recipients, in this particular case they correspond to the
		 * To: and Cc: addressees in the header, but they could be any kind of
		 * recipient. */
		recipients = curl_slist_append(recipients, TO);
		recipients = curl_slist_append(recipients, CC);
		//curl_easy_setopt(curl, CURLOPT_FILE, "message.txt");
		curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

		/* We're using a callback function to specify the payload (the headers and
		 * body of the message). You could just use the CURLOPT_READDATA option to
		 * specify a FILE pointer to read from. */
		//curl_easy_setopt(curl, CURLOPT_INFILESIZE, file_size);
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
		curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

		/* Since the traffic will be encrypted, it is very useful to turn on debug
		 * information within libcurl to see what is happening during the transfer.
		 */
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

		/* Send the message */
		res = curl_easy_perform(curl);

		/* Check for errors */
		if (res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));

		/* Free the list of recipients */
		curl_slist_free_all(recipients);

		/* Always cleanup */
		curl_easy_cleanup(curl);
	}
	return (int)res;
}