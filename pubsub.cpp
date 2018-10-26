/**
 * \brief publish/subscribe sample code
 *
 * This is the beginning of some of my experiments with publish/subscribe model
 * This is not meant as a drop-in, just a playground for now.
 *
 * \author Stephan K. Zitz
 */

#include <bits/stdc++.h>
#include <vector>

using namespace std;


class MessageBase;


/**
 * \class Subscriber
 *
 * Anything that wants to subscribe to a message must
 * descend from this class
 */
class Subscriber {
private:
    unsigned int ID;
    static unsigned int runningID;
public:
    Subscriber() { ID = runningID++; }
    virtual ~Subscriber() = default;
    virtual void notify(MessageBase* ) = 0;

    unsigned int getID() const { return ID; }
};

unsigned int Subscriber::runningID = 0;


/**
 * \class MessageBase
 *
 * Any messages that will be published must descend
 * from this class
 *
 * The "name" parameter is provided as a means to differentiate similar messages
 * with a differentiator.  It is not necessary to use (hence a default constructor
 * is provided)
 *
 * To use, add subscribers via addSubscriber() and when ready to publish
 * this message, call its notify() method.
 *
 * It is possible to overload the notify() method to provide context
 * to a specific message, so long as the overloaded method ultimately
 * calls MessageBase::notfy().
 * For instance:
 * \code{.cpp}
 * void MessageBaseDescendant::notify( ContextType* someContext ) {
 *   setContext(someContext);
 *   MessageBase::notify();
 * }
 * \endcode
*/
class MessageBase {
private:
    vector<Subscriber*> subscribers;
    string              name;

public:
    MessageBase(string _name ) : name(_name){}
    MessageBase() {}
    virtual ~MessageBase() = default;

    void copySubscribers(const MessageBase* from) { subscribers = from->subscribers; }

    void notify( ) {
        for ( vector<Subscriber*>::iterator iter = subscribers.begin();
              iter != subscribers.end();
              ++iter ) {
            (*iter)->notify(this);
        }
    }

    void addSubscriber( Subscriber* pSubscriber ) {
        subscribers.push_back(pSubscriber);
    }

    string getName() { return name;}
    void   setName(string _name) { name = _name;}
};


class TestMessage1 :
    public MessageBase {
public:
    TestMessage1() : MessageBase("TestMessage1") {}

};


class TestMessage2 :
    public MessageBase {
public:
    TestMessage2() : MessageBase("TestMessage2") {}

};


class DynamicMessage :
    public MessageBase {
private:
    string sMessage;

public:
    DynamicMessage(string _sMessage) : MessageBase("DynamicMessage"),sMessage(_sMessage) {}

    const string getString() const { return sMessage; }
};


class NamedSubscriber :
    public Subscriber {
private:
    string subscriberName;

public:
    NamedSubscriber(string _subscriberName) : subscriberName(_subscriberName) {}

    virtual void notify(MessageBase* pMessage) {
        cout << "Subscriber(" << subscriberName << ") notify fired:";
        if ( DynamicMessage* pDynamicMessage = dynamic_cast<DynamicMessage*>(pMessage)) {
            cout << "Received dynamic message.  string=" << pDynamicMessage->getString() << endl;
        } else {
            cout << "Received polymorphic message.  message name = " << pMessage->getName() << endl;
        }
    }
};


// a possible "real world" example
class TransformerAlert :
    public MessageBase {
private:
    string whichTransformer;
public:
    TransformerAlert(string _whichTransformer) :
        MessageBase("Transformer Alert"),
        whichTransformer(_whichTransformer) {}

    virtual void alertText() { cout << "Generic Transformer alert for " << whichTransformer << endl; }
    string getWhichTransformer() { return whichTransformer; }
};

class OvervoltageAlert :
    public TransformerAlert {
private:
    float voltageAmount;

public:
    OvervoltageAlert( string _whichTransformer) :
        TransformerAlert(_whichTransformer) { setName("OvervoltageAlert");}

    virtual void alertText() { cout << "Over voltage by " << voltageAmount << " alert from Transformer " << getWhichTransformer() << endl; }

    void notify( float _voltageAmount ) {
        voltageAmount = _voltageAmount;
        MessageBase::notify();
    }
};

class UndervoltageAlert :
    public TransformerAlert {
public:
    UndervoltageAlert( string _whichTransformer) :
        TransformerAlert(_whichTransformer) { setName("UndervoltageAlert");}

    virtual void alertText() { cout << "Under voltage alert from Transformer " << getWhichTransformer() << endl; }
};


class VoltageAlertHandler :
    public Subscriber {
private:
    string sendTo;
public:
    VoltageAlertHandler(string _sendTo) : sendTo(_sendTo){}
    virtual void notify( MessageBase* pMessage ) {
        if ( TransformerAlert* pAlert = dynamic_cast<TransformerAlert*>(pMessage)) {
            cout << "Sending this transformer alert message to: " << sendTo << endl;
            pAlert->alertText();
        }
    }
};

int main() {
    NamedSubscriber sub1("sub1");
    NamedSubscriber sub2("sub2");
    NamedSubscriber sub3("sub3");

    TestMessage1 message1;
    message1.addSubscriber( &sub1 );

    TestMessage2 message2;
    message2.addSubscriber( &sub2 );
    message2.addSubscriber( &sub3 );

    TestMessage1 message3;
    message3.addSubscriber( &sub1 );
    message3.addSubscriber( &sub2 );
    message3.addSubscriber( &sub3 );

    DynamicMessage message4("Hello World");
    message4.copySubscribers(&message3);

    cout << "Firing message1" << endl;
    message1.notify();

    cout << "Firing message2" << endl;
    message2.notify();

    cout << "Firing message3" << endl;
    message3.notify();

    cout << "Firing message4" << endl;
    message4.notify();

    cout << endl << endl << endl << endl << "Pretend real-world example" << endl;

    VoltageAlertHandler emailHandler("mailto:crises@electrical-company.com");
    VoltageAlertHandler pagerHandler("pager:123123123");
    VoltageAlertHandler urlHandler("http://www.electrical-company.com");

    // now, we bring up our transformers ... and the alerts they can generate
    // Transformer* T1 = new Transformer();
    OvervoltageAlert  T1OV_alert("transformer_id_1");
    UndervoltageAlert T1UV_alert("transformer_id_1");
    T1OV_alert.addSubscriber( &emailHandler );
    T1OV_alert.addSubscriber( &pagerHandler );
    T1OV_alert.addSubscriber( &urlHandler );
    T1UV_alert.copySubscribers( &T1OV_alert );

    // Transformer* T2 = new Transformer();
    OvervoltageAlert  T2OV_alert("transformer_id_2");
    UndervoltageAlert T2UV_alert("transformer_id_2");
    T2OV_alert.copySubscribers( &T1OV_alert);
    T2UV_alert.copySubscribers( &T1OV_alert);
    // now, we're in a loop monitoring ...

    // while (continue_monitoring)

    // uhoh transformer2 is suddenly generating voltages in excess of 150KV
    cout << "***************************" << endl;
    cout << "Transformer #2 over-voltaged by 150KV!!! Alert those who care!" << endl;
    T2OV_alert.notify(150000);
    cout << "***************************" << endl;

    // some time in the future, transformer 1 is suddenly seeing no voltage
    cout << "***************************" << endl;
    cout << "Transformer #1 not generating voltage!!! Alert those who care!" << endl;
    T1UV_alert.notify();
    cout << "***************************" << endl;

    // the NOC now has a added a new email address for these alerts:
    VoltageAlertHandler NOCemailHandler("mailto:NOC@electrical-company.com");
    // add this new email handlers to all the transformers
    T1OV_alert.addSubscriber(&NOCemailHandler);
    T1UV_alert.addSubscriber(&NOCemailHandler);
    T2OV_alert.addSubscriber(&NOCemailHandler);
    T2UV_alert.addSubscriber(&NOCemailHandler);

    // alright, should anything bad happen to our transformers,
    // the NOC will also be notified.

    // some time in the future ...
    // uhoh transformer 1 is suddenly generating voltages in excess of 200KV
    cout << "***************************" << endl;
    cout << "Transformer #1 over-voltaged by 200kV!!! Alert those who care!" << endl;
    T1OV_alert.notify(200000);
    cout << "***************************" << endl;



}
