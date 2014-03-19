/*
  Copyright © 2014 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/MIT

  File: ip.cpp
  Author: Jesse 'Jeaye' Wilkerson
*/

#include <vector>
#include <memory>
#include <array>

#include <curl/curl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "ip.h"

namespace shared
{
  namespace network
  {
    namespace ip
    {
      std::string get_internal()
      {
        std::array<char, 256> hostname; 
        gethostname(hostname.data(), hostname.size());
        hostent * const entry{ gethostbyname(hostname.data()) };

        return { inet_ntoa(*reinterpret_cast<in_addr*>(*entry->h_addr_list)) };
      }

      static size_t writer(void * const contents, size_t const size, size_t const count, void * const userp)
      {
        size_t const realsize{ size * count };
        char * const data{ static_cast<char*>(contents) };
        auto * const buff(static_cast<std::vector<char>*>(userp));
        std::copy(data, data + realsize, std::back_inserter(*buff));

        return realsize;
      }

      std::string get_external()
      {
        std::vector<char> buff;
        std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> curl{ curl_easy_init(), &curl_easy_cleanup };
        if(curl)
        {
          curl_easy_setopt(curl.get(), CURLOPT_URL, "http://ipecho.net/plain");
          curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, &writer);
          curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &buff);

          /* TODO: What if the router redirected to another site? Validate IP? */
          CURLcode const res{ curl_easy_perform(curl.get()) };
          if(res != CURLE_OK)
          { return "ip error"; }
          else
          { return { buff.data() }; }
        }
        else
        { return "ip error"; }
      }
    }
  }
}
