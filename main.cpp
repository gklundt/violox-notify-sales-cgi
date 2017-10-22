#include <iostream>
#include <stdio.h>
#include <string.h>
#include <cgicc/CgiDefs.h>
#include <cgicc/Cgicc.h>
#include <cgicc/HTTPHTMLHeader.h>
#include <cgicc/HTMLClasses.h>


#include <curl/curl.h>

#include <boost/uuid/nil_generator.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/name_generator.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include "boost/date_time/local_time/local_time.hpp"


#define FROM "web@violox.com"
#define TO "sales@violox.com"

static const char *payload_text[] = {NULL, NULL};

struct upload_status {
    int lines_read;
};

static size_t payload_source(void *ptr, size_t size, size_t nmemb, void *userp) {
    struct upload_status *upload_ctx = (struct upload_status *) userp;
    const char *data;

    if ((size == 0) || (nmemb == 0) || ((size * nmemb) < 1)) {
        return 0;
    }

    data = payload_text[upload_ctx->lines_read];

    if (data) {
        size_t len = strlen(data);
        memcpy(ptr, data, len);
        upload_ctx->lines_read++;

        return len;
    }

    return 0;
}

using namespace cgicc;
int main() {

    Cgicc formData;

    std::cout << "Content-type:text/html" << std::endl << std::endl;
    std::cout << "<html>" << std::endl;
    std::cout << "<head>" << std::endl;
    std::cout << "<title>Using GET and POST Methods</title>" << std::endl;
    std::cout << "</head>" << std::endl;
    std::cout << "<body>" << std::endl;


    std::string requestName;
    std::string requestCompany;
    std::string requestTitle;
    std::string requestEmail;
    std::string requestPhone;



    form_iterator fi = formData.getElement("request_name");
    if( !fi->isEmpty() && fi != (*formData).end()) {
        std::cout << "Contact Name: " << **fi << std::endl;
        requestName = **fi;
    } else {
        std::cout << "No text entered for contact name" << std::endl;
        requestName = "";
    }

    std::cout << "<br/>\n";
    fi = formData.getElement("request_company");
    if( !fi->isEmpty() &&fi != (*formData).end()) {
        std::cout << "Company Name: " << **fi << std::endl;
        requestCompany = **fi;
    } else {
        std::cout << "No text entered for company name" << std::endl;
        requestCompany = "";
    }

    std::cout << "<br/>\n";
    fi = formData.getElement("request_title");
    if( !fi->isEmpty() &&fi != (*formData).end()) {
        std::cout << "Title: " << **fi << std::endl;
        requestTitle = **fi;
    } else {
        std::cout << "No text entered for title" << std::endl;
        requestTitle = "";
    }
    std::cout << "<br/>\n";
    fi = formData.getElement("request_email");
    if( !fi->isEmpty() &&fi != (*formData).end()) {
        std::cout << "Email Address: " << **fi << std::endl;
        requestEmail = **fi;
    } else {
        std::cout << "No text entered for email address" << std::endl;
        requestEmail = "";
    }
    std::cout << "<br/>\n";
    fi = formData.getElement("request_phone");
    if( !fi->isEmpty() &&fi != (*formData).end()) {
        std::cout << "Phone Number: " << **fi << std::endl;
        requestPhone = **fi;
    } else {
        std::cout << "No text entered for phone number" << std::endl;
        requestPhone = "";
    }

    std::cout << "<br/>\n";
    std::cout << "</body>\n";
    std::cout << "</html>\n";











    std::ostringstream oss;


    // Set the date
    boost::gregorian::date date = boost::gregorian::date(boost::gregorian::day_clock::local_day());
    boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();

    // Set the message ID
    boost::uuids::random_generator random_generator;
    boost::uuids::uuid theUuid = random_generator();
    const std::string msgUuid = boost::lexical_cast<std::string>(theUuid);


    oss << "Date: ";
    oss << date.day_of_week().as_short_string();
    oss << ", ";
    oss << date.day().as_number();
    oss << " ";
    oss << date.month().as_short_string();
    oss << " ";
    oss << date.year();
    oss << " ";
    oss << now.time_of_day().hours();
    oss << ":";
    oss << now.time_of_day().minutes();
    oss << ":";
    oss << now.time_of_day().seconds();
    oss << " -0600";
    oss << std::endl;


    oss << "To: <sales@violox.com>" << std::endl;
    oss << "From: (Violox Website)" << std::endl;
    oss << "Message-ID:" << msgUuid << "@violox.com" << std::endl;

    oss << "Subject: Web Email - Contact " << requestName << " at " << requestCompany << std::endl << std::endl;
    oss << "Request Form Submitted:" << std::endl;
    oss << "Name: " << requestName << std::endl;
    oss << "Company: " << requestCompany << std::endl;
    oss << "Title: " << requestTitle << std::endl;
    oss << "Email: " << requestEmail << std::endl;
    oss << "Phone: " << requestPhone << std::endl;

    std::string pt = oss.str();

    payload_text[0] = pt.c_str();

    CURL *curl;
    CURLcode res = CURLE_OK;
    struct curl_slist *recipients = NULL;
    struct upload_status upload_ctx;

    upload_ctx.lines_read = 0;

    curl = curl_easy_init();
    if (curl) {
        /* This is the URL for your mailserver */
        curl_easy_setopt(curl, CURLOPT_URL, "smtps://smtp.gmail.com:465");

        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
        curl_easy_setopt(curl, CURLOPT_USERNAME, "web@violox.com");
        curl_easy_setopt(curl, CURLOPT_PASSWORD, "T3<=AyUD");

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
//        recipients = curl_slist_append(recipients, CC);
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);


        /* We're using a callback function to specify the payload (the headers and
         * body of the message). You could just use the CURLOPT_READDATA option to
         * specify a FILE pointer to read from. */
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
        curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

        /* Send the message */
        res = curl_easy_perform(curl);

        /* Check for errors */
        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));

        /* Free the list of recipients */
        curl_slist_free_all(recipients);

        /* curl won't send the QUIT command until you call cleanup, so you should
         * be able to re-use this connection for additional messages (setting
         * CURLOPT_MAIL_FROM and CURLOPT_MAIL_RCPT as required, and calling
         * curl_easy_perform() again. It may not be a good idea to keep the
         * connection open for a very long time though (more than a few minutes
         * may result in the server timing out the connection), and you do want to
         * clean up in the end.
         */
        curl_easy_cleanup(curl);
    }

    return (int) res;
}
