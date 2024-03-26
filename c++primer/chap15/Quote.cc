#include <algorithm>
#include <cstddef>
#include <iostream>
#include <memory>
#include <ostream>
#include <set>
#include <string>
#include <utility>
class Quote {
public:
  Quote() = default;
  Quote(const std::string &book, double sales_price)
      : bookNo(book), price(sales_price) {}
  Quote(const Quote &q) : bookNo(q.bookNo), price(q.price) {}
  Quote &operator=(const Quote &q) {
    bookNo = q.bookNo;
    price = q.price;
    return *this;
  }
  std::string isbn() const { return bookNo; }
  virtual double net_price(std::size_t n) const { return n * price; }
  virtual ~Quote() = default;
  virtual void debug() const {
    std::cout << "bookNo :" << bookNo << ", "
              << "price " << price << " ";
  }
  virtual Quote *clone() const & { return new Quote(*this); }
  virtual Quote *clone() && { return new Quote(std::move(*this)); }

private:
  std::string bookNo;

protected:
  double price = 0.0;
};

double print_total(std::ostream &os, const Quote &item, std::size_t n) {
  double ret = item.net_price(n);
  os << "isbn: " << item.isbn() << " $ sold: " << n << " total due: " << ret
     << "\n";
  return ret;
}

class Disc_quote : public Quote {
public:
  Disc_quote() = default;
  Disc_quote(const std::string &str, double price, std::size_t n, double disc)
      : Quote(str, price), quantity(n), discount(disc) {}
  Disc_quote(const Disc_quote &q)
      : Quote(q), quantity(q.quantity), discount(q.discount) {}
  double net_price(std::size_t n) const = 0;
  virtual void debug() const {
    Quote::debug();
    std::cout << "quantity: " << quantity << ", "
              << "discount: " << discount << " ";
  }

protected:
  std::size_t quantity = 0;
  double discount = 0.0;
};
class Limited_quote : public Disc_quote {
public:
  using Disc_quote::Disc_quote;
  double net_price(std::size_t n) const override {
    if (n >= quantity) {
      return quantity * (1 - discount) * price + (n - quantity) * price;
    }
    return n * (1 - discount) * price;
  }
  void debug() const override {
    std::cout << "Limited_quote: ";
    Disc_quote::debug();
  }
  Limited_quote *clone() const & override {
    auto p = new Limited_quote(*this);
    return p;
  }
  Limited_quote *clone() && override {
    auto p = new Limited_quote(std::move(*this));
    return p;
  }

private:
};

class Bulk_quote : public Disc_quote {
public:
  using Disc_quote::Disc_quote;
  double net_price(std::size_t n) const override {
    if (n > quantity) {
      return n * price;
    }
    return n * (1 - discount) * price;
  }
  void debug() const override { Disc_quote::debug(); }
  Bulk_quote *clone() const & override {
    auto p = new Bulk_quote(*this);
    return p;
  }
  Bulk_quote *clone() && override {
    auto p = new Bulk_quote(std::move(*this));
    return p;
  }

private:
};

class Basket {
public:
  Basket() = default;
  void add_item(const Quote &sale);
  void add_item(Quote &&sale);
  double total_receipt(std::ostream &os) const;

private:
  static bool compare(const std::shared_ptr<Quote> &lhs,
                      const std::shared_ptr<Quote> &rhs) {
    return lhs->isbn() < rhs->isbn();
  }
  std::multiset<std::shared_ptr<Quote>, decltype(compare) *> item{compare};
};
double Basket::total_receipt(std::ostream &os) const {
  double sum = 0;
  for (auto iter = item.cbegin(); iter != item.cend(); ++iter) {
    sum += print_total(os, **iter, item.count(*iter));
  }
  os << "total sale: " << sum << "\n";
  return sum;
}
void Basket::add_item(const Quote &sale) {
  auto p = std::shared_ptr<Quote>(sale.clone());
  auto i = item.insert(p);
}
void Basket::add_item(Quote &&sale) {
  auto p = std::shared_ptr<Quote>(std::move(sale).clone());
  auto i = item.insert(p);
}
int main(int argc, char *argv[]) {
  Bulk_quote bq("c++ primer", 100, 20, 0.2);
  Quote q("effective modern c++", 40);
  Quote qq("effective modern c++", 40);
  Quote qqq("effective modern c++", 40);
  Limited_quote lq("effective c++", 20, 20, 0.2);

  Basket b;
  b.add_item(q);
  b.add_item(lq); // 本行代码会发生段错误
  b.add_item(bq);
  b.total_receipt(std::cout);

  return 0;
}
