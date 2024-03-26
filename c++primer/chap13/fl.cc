#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
class Message;
class Folder {
  friend class Message;

public:
  void addMsg(Message *m) { msgs.insert(m); }
  void remMsg(Message *m) { msgs.erase(m); }

private:
  std::set<Message *> msgs;
};

class Message {
  friend class Folder;
  friend void swap(Message &, Message &);

public:
  explicit Message(const std::string &str = "") : contents(str) {}
  Message(const Message &);
  Message(Message &&);
  Message &operator=(const Message &);
  Message &operator=(Message &&);
  ~Message();
  void save(Folder &);
  void remove(Folder &);
  void addfolder(Folder *);
  void remfolder(Folder *);
  void movefolders(Message *);

private:
  std::string contents;
  std::set<Folder *> folders;
  void add_to_folders(const Message &);
  void remove_from_folders();
};
void Message::addfolder(Folder *f) { folders.insert(f); }
void Message::remfolder(Folder *f) { folders.erase(f); }
void Message::save(Folder &f) {
  folders.insert(&f);
  f.addMsg(this);
}
void Message::remove(Folder &f) {
  folders.erase(&f);
  f.remMsg(this);
}
void Message::add_to_folders(const Message &m) {
  for (auto f : m.folders) {
    f->addMsg(this);
  }
}
Message::Message(const Message &m) : contents(m.contents), folders(m.folders) {
  add_to_folders(m);
}

Message::Message(Message &&m) : contents(std::move(m.contents)) {
  movefolders(&m);
}

void Message::remove_from_folders() {
  for (auto i : folders) {
    i->remMsg(this);
  }
}
Message::~Message() { remove_from_folders(); }
Message &Message::operator=(const Message &m) {
  remove_from_folders();
  contents = m.contents;
  folders = m.folders;
  add_to_folders(m);
  return *this;
}
Message &Message::operator=(Message &&m) {
  if (&m == this)
    return *this;
  remove_from_folders();
  contents = std::move(m.contents);
  movefolders(&m);
  return *this;
}
void Message::movefolders(Message *m) {
  folders = std::move(m->folders);
  for (auto f : folders) {
    f->remMsg(m);
    f->addMsg(this);
  }
  m->folders.clear();
}

void swap(Message &m1, Message &m2) {
  using std::swap;
  for (auto f : m1.folders) {
    f->remMsg(&m1);
  }
  for (auto f : m2.folders) {
    f->remMsg(&m2);
  }
  swap(m1.folders, m2.folders);
  swap(m1.contents, m2.contents);
  for (auto f : m2.folders) {
    f->addMsg(&m2);
  }
  for (auto f : m1.folders) {
    f->addMsg(&m1);
  }
}
int main(int argc, char *argv[]) {
  std::vector<Folder> vec(5);
  int index;
  std::string line;
  while (std::getline(std::cin, line)) {
    std::istringstream iss(line);
    Message *msg = new Message(line);
    while (iss >> index) {
      msg->save(vec[index]);
    }
  }

  return 0;
}
