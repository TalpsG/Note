
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
int main(int argc, char *argv[]) {
  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(1234);
  addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  auto fd = socket(AF_INET, SOCK_STREAM, 0);
  auto ret = connect(fd, (sockaddr *)&addr, sizeof(addr));
  std::string str("talps hello");
  write(fd, str.data(), str.size());
  return 0;
}
