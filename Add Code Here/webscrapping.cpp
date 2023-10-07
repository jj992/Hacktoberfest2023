#include <iostream>
#include <curl/curl.h>
#include "libxml/HTMLparser.h"
#include "libxml/xpath.h"

using namespace std;

int main() {
  // Create a curl handle.
  CURL *curl = curl_easy_init();

  // Set the URL to scrape.
  curl_easy_setopt(curl, CURLOPT_URL, "https://example.com");

  // Perform the HTTP request.
  CURLcode res = curl_easy_perform(curl);

  // Check if the request was successful.
  if (res != CURLE_OK) {
    cerr << "curl error: " << curl_easy_strerror(res) << endl;
    return 1;
  }

  // Get the response body.
  string responseBody;
  CURLcode writeRes = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBody);
  curl_easy_perform(curl);

  // Close the curl handle.
  curl_easy_cleanup(curl);

  // Parse the HTML document.
  htmlDocPtr doc = htmlReadMemory(responseBody.c_str(), responseBody.length(), NULL, "UTF-8", HTML_PARSE_RECOVER);

  // Extract the data that you want to scrape.
  xmlXPathContextPtr xpathCtx = xmlXPathNewContext(doc);
  xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression("/html/body/h1/text()", xpathCtx);

  // Get the text of the first h1 element.
  xmlChar *text = xmlNodeGetContent(xpathObj->nodesetval->nodeTab[0]);

  // Print the text to the console.
  cout << text << endl;

  // Free the XML objects.
  xmlXPathFreeObject(xpathObj);
  xmlXPathFreeContext(xpathCtx);
  xmlFreeDoc(doc);

  return 0;
}

size_t WriteCallback(void *ptr, size_t size, size_t nmemb, void *stream) {
  string *responseBody = (string *)stream;
  responseBody->append((char *)ptr, size * nmemb);
  return size * nmemb;
}
