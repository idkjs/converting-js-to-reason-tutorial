# Converting Js to ReasonML - Part 4

## Converting auth.js to Auth.re

![Kublai Kahn](https://raw.githubusercontent.com/idkjs/converting-js-to-reason-tutorial/master/images/medium-header.jpeg)

In [Part 2](), we wrote bindings in `Header.re` to the `auth.js` file that came with the boilerplate. The point was to show you how to write bindings. In this post, we will rewrite `auth.js` in Reason. But first...

## The Reason Community

The ReasonML community is the best every. As a clown, big time imposter syndrome programmer, there was no better cure for me than the [Reason Community resources](https://reasonml.github.io/docs/en/community).

Everything I am able to tell you or even remotely seem like I understand is a direct result of this community to whom I hereby, publicly and sincerely say, thank you.

So in the previous post, I was going to bind the `react-helmet` library and then re-import it to `layout.js` just to get you one more look at writing a binding. This meant, binding the js version of `react-helmut` in Reason and re-importing into `layout.js`. This is totally doable and there is an example in the reason materials. Still, I didn't want to waste to much time on it so I asked in the [ReasonML](https://discordapp.com/channels/235176658175262720/235176658175262720) discord channel. My man, [@yawaramin](https://mobile.twitter.com/yawaramin) quickly provided the following answer:

> well, components wrapped using wrapJsForReason are meant to be used by Reason code, not by JS code. they already exist as JS code so you would just use e.g. import Helmet from 'react-helmet'

I decided then that you all got the point on bindings for now and to move on to the next thing.

## Back to Auth.re

`auth.js` is a simple enough looking file. Here it is:

```js
const isBrowser = typeof window !== `undefined`

export const setUser = user => (window.localStorage.gatsbyUser = JSON.stringify(user))

const getUser = () =>
  window.localStorage.gatsbyUser
    ? JSON.parse(window.localStorage.gatsbyUser)
    : {}

export const isLoggedIn = () => {
  if (!isBrowser) return false

  const user = getUser()
  if (user) return !!user.username
}

export const getCurrentUser = () => isBrowser && getUser()

export const logout = callback => {
  if (!isBrowser) return
  setUser({})
  callback()
}
```

Its six statements, four of which are exported. I saw this and said, this will be easy. As soon as I said that, I should have known I was wrong. This took me for a spin for about a day. I could not let it go and so I followed my programming rules. What are the rules? Well among them is, "Ask the damned question if you can't figure it out!". It will take you a lot less time in the Reason community because these guys make it their business to help. Additionally, you always learn something more by just studying how whoever is helping you did it. So I asked the question [here](https://reasonml.chat/t/help-converting-the-js-to-reasonml/1438?u=idkjs). I got two quick responses from [@yawaramin](https://mobile.twitter.com/yawaramin) and [@bsansouci](https://mobile.twitter.com/bsansouci), two of the baddest reasonml/ocaml devs out there. bsansouci might even be on the reasonml team, I don't know.

This is what @yawaramin had to [say](https://reasonml.chat/t/help-converting-the-js-to-reasonml/1438/2?u=idkjs):

>The Reason code you have now doesn’t exactly match up with the original JavaScript. For example, looking at the JavaScript, the isLoggedIn function should have the following type in Reason: let isLoggedIn: unit => bool.

> If we were to do a somewhat literal translation of the JavaScript code, the Reason should look like this:

```reason
/* Assuming the following are available:
let isBrowser: bool;
let getUser: unit => Js.nullable({.. "username": Js.nullable(string)}); */

let isLoggedIn() = isBrowser && {
  ()
  |> getUser
  |> Js.Nullable.toOption
  |> Js.Option.andThen((. user) =>
    Js.Nullable.toOption(user##username))
  |> Js.Option.isSome
};
```

> This can definitely be improved further but it’s a good start. The key is to model the data types correctly, especially the type of getUser. Once you know those, you can manipulate the data using the standard functions shipped with BuckleScript to access the possibly-undefined values in a safe way.

And this what @bsansouci had to [say](https://reasonml.chat/t/help-converting-the-js-to-reasonml/1438/3?u=idkjs):

> I’d do something simple like:

```reason
let isBrowser = true; /* Do your logic thing here */

let getUser = () => {
  let user = Dom.Storage.(localStorage |> getItem("gatsbyUser"));
  switch user {
  | None => Js.Obj.empty()
  | Some(user) => user
  }
};

let isLoggedIn = () => {
  if (!isBrowser) {
    false
  } else {
    switch (getUser()) {
      | None => false
      | Some(user) =>
        switch (Js.undefinedToOption(user##username)) {
          | None => false
          | Some(username) => Js.String.length(username) > 0
        }
     }
```

Let's do the work. Create an `AuthRe.re` file next to `auth.js`. We are calling it `AuthRe.re` for now so that the name space doesn't clash with `auth.js` when gatsby's bundler builds.

The first thing we need to do is determine if the client is in the browser or not. In js it was done like this:

```js
const isBrowser = typeof window !== `undefined`
```

In reason we will write a binding to some value available in the global scope in the browser api which can be reviewed [here](https://www.w3schools.com/js/js_window.asp) and [here](https://developer.mozilla.org/en-US/docs/Web/API/Window). These are basically functions and values that exist in the browser. Open the console in your browser and type `Object.keys( window );` to see all the global values. In js, the variable `isBrowser` checked if the `window` type is undefined which means we are checking if it exists. Obvioiusly, if its not undefined, then it exists and we must therefore be in a browser environment. Keeping with our talking it out method:
> Hey bucklescript there is a global object called window with a bunch of values we want to get a value that we are calling isBrowser that has a type bool that is equal to the window object. So if there is a window object, return true, if there is no window object, return false. So isBrowser should be true or false.

```reason
[@bs.val] external isBrowser: bool = "window";
```

Go into `pages/indexPage.re` and add `AuthRe.isBrowser |> Js.log;` to the top of the page. Run `yarn watch && yarn dev` if you haven't already, open the browser console. You should see:

```sh
Window http://localhost:8000/
```

So when we are calling `isBrowser` we get a string. This is not what I expected. I thought we were going to get true or false. So I then fished through some old code that actually got me back true and false. Here it is:

```reason
[@bs.val] external browserEnv: bool = "process.browser";

/* add following to `indexPage.re` */
AuthRe.browserEnv |> Js.log;
```

In browser console, this outputs:`true`. I just wasted a bunch of time trying to find the node.js `process.browser` api so you and I might have a source of truth on this and could not find it. It works, though, so lets go with it for now. This, my friends, is programming.

## Auth.re Reset

Let's forget all that above. I am going to leave it there because there may be some tidbits in there worth exploring. I could not get it to work and so I'm going to show you what I actually did.

In `AuthRe.re` we are going to create a module to determine what environment we are in. We want to know if we are in the `Browser` or in `Node`. So that sounds like a good place to use a [Variant](https://reasonml.github.io/docs/en/variant#docsNav).

First we will use [@bs.val] to set a value for the `window` so we can call it later.

Let's create our module then get create the window value:

```reason
module Env = {
  [@bs.val] external window: 'a = "window";
  [@bs.val] external isBrowser: bool = "process.browser";
};
```

As we saw previously, if we do `AuthRe.Env.window |> Js.log;` we will get the url for the window in the browser console.
As we saw previously, if we do `AuthRe.Env.isBrowser |> Js.log;` we will get true or false in the browser console.

Let's create a variant. Straight from the docs:

> Most data structures in most languages are about "this and that". A variant allows us to express "this or that".

So our variant is about `Browser` or `Node`. Add this to our `Env` module:

```reason
type platform =
    | Browser
    | Node;
```

`Browser` and `Node` aren't strings, nor references, nor some special data type. They're called "constructors" (or "tag"). The `|` bar separates each constructor.

Now we need to use our variant. Docs again:

> Along with a variant comes one of the most important features of Reason, the switch expression.

> A Reason switch is visually similar to other languages' switch (aka a large if/elseif/elseif...). It allows you to check every possible case of a variant. To use it, enumerate every variant constructor of the particular variant you'd like to use, each followed by an => and the expression corresponding to that case.

So every possibility in this case is just `Browser` and `Node`.

Let add a switch statement that uses every possibility.

```reason
 let getPlatform = () => switch (isBrowser) {
   | (true) => Browser
   | (false) => Node
 };

 /* once you hit save, this will reformat to this ternary which reads "if true return Browser, if false, return Node. Same thing but cleaner. */

 let getPlatform = () => isBrowser ? Browser : Node;
```

So we know have a function that will tell us that which environment we are in. The `(true) and (false)` are called [pattern matching](https://reasonml.github.io/docs/en/pattern-matching). We are passing in the possible result of isBrowser, which we know are `true` or `false` and based on the result returning one of the variant options. `getPlatform` doesn't seem to do much on its own. We will now use it to get our logged in user from localStorage.

```reason

let getStore = identifier =>
  switch (Env.getPlatform()) {
  | Browser => Dom.Storage.(localStorage |> getItem(identifier))
  | _ => None
  };

[@bs.deriving abstract]
type store = {
   username: string,
   email: string,
   phone_number: string,
   phone_number_verified: string,
};

let store =
  switch (getStore("gatsbyUser")) {
  | Some(storeValues) =>
    try (Js.Json.parseExn(storeValues)->Obj.magic) {
    | exn =>
      Js.log2("Failed to parse store", exn);
      signOut();
    }
  | None => signOut()
  };

```

What is going on here? First we need a type for our store values which we called `store`. If we look at the Amplify docs or just `console.log` the return value in `SignUp.js` or `SignIn.js` we will see a the json values beind saved in localStorage. It looks like this on my machine:

```json
{
  "email_verified": true,
  "phone_number_verified": false,
  "phone_number": "+5555555555",
  "email": "email@gmail.com",
  "username": "idkjs"
}
```

So we use that to model our `store` type which looks like this:

```reason
[@bs.deriving abstract]
type store = {
   username: string,
   email: string,
   phone_number: string,
   phone_number_verified: string,
};
```

The `bs.deriving abstract` is a helper that converts back and forth between Records in Reason and JavaScript Objects. Docs are [here](https://bucklescript.github.io/docs/en/object#record-mode).

Our `store` variable will now be inferred to be of type `store`. So it will expect the result to be of the store type which I guess could be of either a record or js since both are available with bs.deriving abstract(?). Nice.

The `getStore` function takes and identifier which is the string that identifies the key in localStorage that we want to retrieve. We could have hard coded it but this way its more reusable. `getStore` will check which platform we are on and switch based on our platform variant. If we are in the browser, it will try to retrieve the stored values for `gatsby` that we passed in. If we are in anything else, which is what `_`, says, return `Node`. So when we call `getStore` from `let store` we are going to switch based on what is returned. If `getStore` returns values, then we know we are in the browser and we should try to parse the returned values. If we get `None` back, well then no one is signed in and we call a `signOut` function to make sure the store is clear.

So much going on here. I'm going to stop discussing this part of the `AuthRe.re`. Please check out the code for this part at this tag: [creating-auth-re](https://github.com/idkjs/converting-js-to-reason-tutorial/tree/creating-auth-re) in the repo and ask any questions you might have. Don't be shy. We will both learn something from your questions.

Note: It turns out that even though the code worked, `[@bs.val] external isBrowser: bool = "process.browser";` is not a thing because `process.browser` does not exist!

After discussing with "the community" in the  [ReasonChat](https://reasonml.chat/t/help-converting-the-js-to-reasonml/1438/8?u=idkjs) I ended up changing it the following which does the same thing expect uses a real api!.

```reason
const isBrowser = typeof window !== `undefined`
```

Thank [@yawaramin](https://mobile.twitter.com/yawaramin) again.