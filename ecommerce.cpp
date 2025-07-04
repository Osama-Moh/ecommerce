#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <ctime>
#include <stdexcept>
#include <tuple>

using namespace std;

class Expirable{
    
    public:
        virtual bool isExpirable() const = 0; 
        virtual bool hasExpired() const = 0;

        virtual ~Expirable() {
            cout << "Expirable destructor called" << endl;
        } 
};

class Shippable{

    public:
        virtual bool isShippable() const = 0;
        virtual double getWeight() const = 0;
        virtual string getName() const = 0;        

        virtual ~Shippable() {
            cout << "Shippable destructor called" << endl;
        } 
};


class Product {
private:
    string name;
    double price;
    int quantity;

public:
    Product(string n, double p, int q){
        name = n;
        price = p;
        quantity = q;
    }

    string getProductName() const {
        return name;
    }

    double getProductPrice() const {
        return price;
    }

    int getProductQuantity() const {
        return quantity;
    }

    void addQuantity(int q) {
        quantity += q;
    }

    void changePrice(double p) {
        price = p;
    }

    void removeQuantity(int q) {
        quantity -= q;
    }

    bool isAvailable(int checkAmount) const {
        return quantity >= checkAmount;
    }

    virtual ~Product() {
        cout << "Product destructor called for " << name << endl;
    }    
};

using namespace std;

class TheProduct : public Product, public Shippable, public Expirable {
private:
    double weight = 0.0;
    bool expirable = false;
    bool expired = false;
    
public:
    TheProduct(string n, double p, int q, double w, bool expirable, bool expired = false)
        : Product(n, p, q) {
        weight = w;
        this->expirable = expirable;
        this->expired = expired;
    }

    double getWeight() const override {
        return weight;
    }

    string getName() const override {
        return getProductName();
    }

    bool isExpirable() const override {
        return expirable;
    }
    
    bool isShippable() const override {
        return weight > 0.0;
    } 
    
    bool hasExpired() const override {
        return expired;
    }

    ~TheProduct() override {
        cout << "TheProduct destructor called for " << getProductName() << endl;
    }

};

class Cart{
private:
    vector<tuple<Product*, int>> products;
    double totalPrice;

public:
    Cart() {
        totalPrice = 0.0;
    }

    void addProduct(Product* product, int quantity) {
        if(!product->isAvailable(quantity) || quantity <= 0) {
            throw runtime_error("Insufficient stock for product: " + product->getProductName());
        }

        Expirable* expirableProduct = dynamic_cast<Expirable*>(product);
        if (expirableProduct && expirableProduct->isExpirable() && expirableProduct->hasExpired()) {
            throw runtime_error("Product " + product->getProductName() + " has expired and cannot be added to the cart.");
        }

        products.push_back(make_tuple(product, quantity));
        totalPrice += product->getProductPrice() * quantity;
    }

    double getTotalPrice() const {
        return totalPrice;
    }

    bool isEmpty() const {
        return products.empty();
    }

    vector<tuple<Product*, int>> getProducts() const {
        return products;
    }

    virtual ~Cart() {
        cout << "Cart destructor called." << endl;
    }

};

class ShippingService {
public:
    static void shipProduct(const vector<Shippable*>& products) {
        for (int i = 0; i < products.size(); i++) {
            cout << "Shipping product: " << products[i]->getName() 
                 << " and with weight: " << products[i]->getWeight() << endl;
        }
    }
};

class Customer{
private:
    double balance;
    string name;
    Cart mycart;    

public:
    Customer(string name, double balance) {
        this->name = name;
        this->balance = balance;
    }

    void addToCart(Product* product, int quantity) {
        mycart.addProduct(product, quantity);
    }

    void checkout() {
        if(mycart.isEmpty()) {
            cout << "Your cart is empty." << endl;
            return;
        }
        
        double total = mycart.getTotalPrice();
        double shippingCost = 10.0; 
        vector<Shippable*> shippableProducts;

        if(total > (balance + shippingCost)) {
            cout << "Insufficient balance." << endl;
            return;
        }
        for (int i = 0; i < mycart.getProducts().size(); i++) {
            Product* product = get<0>(mycart.getProducts()[i]);
            int quantity = get<1>(mycart.getProducts()[i]);
            product->removeQuantity(quantity);
            Shippable* shippableProduct = dynamic_cast<Shippable*>(product);
            if (shippableProduct) {
                shippableProducts.push_back(shippableProduct); 
            }
        }
        balance -= total + shippingCost;

        cout << "Order Subtotal: " << total << endl;
        cout << "Shipping Cost: " << shippingCost << endl;
        cout << "Total Amount: " << shippingCost + total <<endl;
        cout << "Checkout successful! Your new balance is: " << balance << endl;

        if(!shippableProducts.empty()) {
            ShippingService::shipProduct(shippableProducts);
        } 
    }
};

int main()
{
    TheProduct product1("Laptop", 1000, 10, 2.5, false);
    TheProduct product2("Tablet", 500, 20, 0.8, false);
    TheProduct product3("Cheese", 5.2, 100, 0.1, true, false);

    Customer customer("Alice", 100);

    customer.addToCart(&product1, 1);
    customer.addToCart(&product2, 2);
    customer.addToCart(&product3, 3);

    customer.checkout();

    return 0;
    
}