#include <curlpp/Easy.hpp>
#include <curlpp/Exception.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/cURLpp.hpp>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>

using json = nlohmann::json;

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <username>" << std::endl;
    return 1;
  }
  try {
    curlpp::Cleanup cleanup;
    curlpp::Easy request;

    std::ostringstream response;
    std::string username = argv[1];
    std::string api_request = "gh api";
    std::list<std::string> header = {"User-Agent: github-activity-script",
                                     "Accept: application/vnd.github+json"};
    std::string method = "--method GET";
    std::string url = "https://api.github.com/users/" + username + "/events";

    curlpp::options::Url my_url(url);
    request.setOpt(curlpp::options::Url(url));
    request.setOpt(curlpp::options::HttpHeader(header));
    // request.setOpt(curlpp::options::Verbose(true));
    request.setOpt(curlpp::options::WriteStream(&response));

    request.perform();

    auto data = json::parse(response.str());

    // Output:
    //- Pushed 3 commits to kamranahmedse/developer-roadmap
    //- Opened a new issue in kamranahmedse/developer-roadmap
    //- Starred kamranahmedse/developer-roadmap

    // PublicEvent

    std::map<std::string, int> push_events;
    std::map<std::string, std::map<std::string, int>> pull_request_events;
    std::map<std::string, int> issue_comment_events;
    for (const auto &event : data) {
      std::string type = event.value("type", "");
      std::string repo = event["repo"].value("name", "");
      std::string action = event["payload"].value("action", "");
      if (type == "PushEvent") {
        push_events[repo]++;
      } else if (type == "PullRequestEvent") {
        pull_request_events[repo][action]++;
      } else if (type == "IssueCommentEvent") {
        issue_comment_events[repo]++;
      } else if (type == "CreateEvent") {
        std::cout << "-- Created a new repository: " << repo << std::endl;
      } else if (type == "WatchEvent") {
        std::cout << "-- Starred a repository: " << repo << std::endl;
      } else if (type == "PublicEvent") {
        std::cout << "-- Made a public repository: " << repo << std::endl;
      } else {
        std::cout << "-- Unknown event type: " << type << std::endl;
      }
    }
    for (const auto &event : push_events) {
      std::cout << "-- Pushed " << event.second << " commits to " << event.first
                << std::endl;
    }
    for (const auto &event : pull_request_events) {
      std::cout << "-- Pull request event for " << event.first << std::endl;
      for (const auto &[action, count] : event.second) {
        std::cout << "---- " << action << ": " << count << std::endl;
      }
    }

    for (const auto &event : issue_comment_events) {
      std::cout << "-- Commented on an issue in " << event.first << std::endl;
    }
  } catch (curlpp::LogicError &e) {
    std::cerr << "Logic error: " << e.what() << std::endl;
  } catch (curlpp::RuntimeError &e) {
    std::cerr << "Runtime error: " << e.what() << std::endl;
  } catch (json::parse_error &e) {
    std::cerr << "JSON parse error: " << e.what() << std::endl;
  }
  return 0;
}
