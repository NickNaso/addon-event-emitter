# Event Emitter on Node.js Native Addons using N-API

## How to create and use **[Event Emitter](https://nodejs.org/dist/latest/docs/api/events.html)** on Node.js Native Addons

Node.js includes an **[event system](https://nodejs.org/dist/latest/docs/api/events.html)**
which is an implementation of the **Observer** pattern that is the most common used
**event** pattern in Node.js application and for good reasons: it's incredibly
simple and useful. 

Anyone who has used Node.js should be familiar with **[events](https://nodejs.org/dist/latest/docs/api/events.html)**.
This module has the responsability to facilitate the communication between **objects**
of your application.

> Much of the Node.js core API modules are built aroud an idiomatic asynchronous
> event-driven architecture in which certains kinds of objects (called **emitter**)
> periodically emit named events that cause Function objects ("listeners") to be called.

Sometimes you need to promote loose coupling system by ensuring that instead of 
components (modules) referring to each other explicitly, their interaction is 
handled through an exchange of message using the **events**. 
This can help to decouple the systems and improve the module reusability.

Here you can find a collection of examples where I used or implemented **event
emitter** interface in a Node.js Native AddOns.

In each example directory, run:

```text
$ npm install
$ node-gyp rebuild
$ node index.js or npm start
```

## Examples

* **[Emit event from C++](/00)**
* **[Implement a Native Addons object that inherit from Event Emitter](/01)**

### Node.js C / C ++ Addons

#### [Documenttation of Node.js Native Addons](https://nodejs.org/dist/latest/docs/api/addons.html)

### N-API 

#### [Documentation for N-API](https://nodejs.org/dist/latest/docs/api/n-api.html)

### Node Addon API
#### Header-only C++ wrapper classes for the ABI-stable Node.js API
#### [Node.js API (N-API) Package](https://github.com/nodejs/node-addon-api/)

### Web resources

* **[How I ported bcrypt to new N-API](https://medium.com/the-node-js-collection/how-i-ported-bcrypt-to-new-n-api-d0b8c9fe6136)**
* **[N-API the next API for Native Addons](https://youtu.be/-Oniup60Afs)**
* **[N-API Next generation Node API for native modules](https://www.slideshare.net/michaeldawson3572846/n-apinode-summit2017final)**
* **[Speed up Your Node.js App with Native Addons](https://medium.com/the-node-js-collection/speed-up-your-node-js-app-with-native-addons-5e76a06f4a40)**
* **[Start with N-API](https://hackernoon.com/n-api-and-getting-started-with-writing-c-addons-for-node-js-cf061b3eae75)**
* **[How to get a performance boost using Node.js native addons](https://medium.com/developers-writing/how-to-get-a-performance-boost-using-node-js-native-addons-fd3a24719c85)**

### Nicola Del Gobbo

<https://github.com/NickNaso/>

<https://www.npmjs.com/~nicknaso>

<https://twitter.com/NickNaso>

## Acknowledgements

Thank you to all people that encourage me every day.

## License

Licensed under [Apache license V2](./LICENSE)
