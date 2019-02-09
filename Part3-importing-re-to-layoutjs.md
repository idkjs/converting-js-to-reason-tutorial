# Converting JS to ReasonML with **Bindings** - Part 3

## Importing Header.re to Layout.js Component

![Kublai Kahn](https://raw.githubusercontent.com/idkjs/converting-js-to-reason-tutorial/master/images/medium-header.jpeg)

## Where Do We Start

If we take a look at the imports on `layout.js` we see a few things we need to handle.

```js
import Helmet from 'react-helmet';
import { StaticQuery, graphql } from 'gatsby';
import Header from './header';
import './layout.css';
```

So we need to a `StaticQuery`, `Header`, `Helmet` and to import some css.

## Header

Let's start with creating the ReasonML version of `Header.re`.

Create `Header.re` in the `components` directory.

These are the functions we need to convert.
```js
import { Link } from 'gatsby'
import { navigate } from '@reach/router'
import { logout, isLoggedIn } from "../utils/auth"
import { Auth } from 'aws-amplify'
```

We already got the `Link` from our our `reason-gatsby-link` import so we dont have to work about that.

The last three we will create using bucklescript like we did before.

### ReachRouter's Navigate function

For `navigate` from `@reach/router` we don't even have to look anything up.
We know there is an export called `navigate` on the `@reach/router` module. So let try talking through the binding outloud.

"bucklescript get module `@reach/router` and access it with a function called `navigate` that takes a string. When you get the string, call the function on the `@reach/router` `navigate` function. `unit` is the keyword that calls the function for us."

And this is what it looks like in code:

```reason
[@bs.module "@reach/router"] external navigate: string => unit = "navigate";
```

### The `auth.js` **binding**

The `auth.js` is a module like any other. Instead of importing it, you wrote it yourself.  Instead of importing it with npm, you wrote it yourself, or in this case, it came with [@dabit3's](https://mobile.twitter.com/dabit3/) boilerplate. So we call it in Reason the exact same way as if we were to call an imported dependency. Talking through it out loud,

 > bucklescript get the module `auth` at `../utils/auth` path and access it with a function called `logout` that takes any old type `'a`. When you get the `'a` thingy, call the function on the `../utils/auth` module with the name `logout`. I'm giving you the `'a` type because this function takes a call back and I actually don't know how to type that correctly just yet so just take any old thing until i figure it out.

For `isLoggedIn` create a function called `isLoggedIn` which expects to get  a boolean type back. Everytime I call it, immediately hit `isLoggedIn` at `../utils/auth` which should return a true or false."

This is the code version of those two statements:

```reason
[@bs.module "../utils/auth"] external logout: ('a, unit) => unit = "logout";
[@bs.module "../utils/auth"] external isLoggedIn: unit => bool = "isLoggedIn";
```

This is the `logout` function in `auth.js`:

```js
export const logout = callback => {
  if (!isBrowser) return
  setUser({})
  callback()
}
```

We see that it takes a callback so I know that I have to pass it one. I am not sure how to type it which is why I used the `'a` type annotation which basically means take anything at all.

In `Header.js` `logout` was called like this:

```js
onClick={
  () => Auth.signOut().then(logout(() => navigate('/app/login'))).catch(err => console.log('eror:', err))
}
```

Reading this, I see that first we are calling `signOut` on the Amplify module. Just to mirror the `Header.js` file so you could see what is going on, I ripped it from `Amplify.re` and redefined it here. Later we will just call it from there.

```reason
type amplify;
[@bs.module "@aws-amplify/auth/lib"] external amplify: amplify = "default";
[@bs.send] external signOut: (amplify, unit) => unit = "signOut";
```

So, we are calling `signOut` then logging the user out by clearing out the local store on the local machine. If that works, call `navigate` to take the person back to the `app/login` page and log any errors.

To do this, I had to go find some code I read somewhere on git hub showing how to do a js style try/catch statement. I haven't seen that documented anywhere so that is a great example of why reading other people's code helps you solve problems. I saw how it was used [here](https://github.com/xodio/xod/blob/ff7cd119660afbf4b0ea62dddff55f5087d8f150/packages/xod-tabtest/src/Tabtest.re#L87-L90) and came up with this:

```reason
let callback =
      try (() => navigate("/app/login")) {
      | Js.Exn.Error(e) => (() => Js.log2("error", e))
      };
```

Which I passed to the `onClick` function here:

```reason
{isLoggedIn() ?
           <p
             style=Styles.link
             onClick={_event =>
               signOut(amplify, ()) |> (() => logout(callback, ()))
             }>
             {ReasonReact.string("SignOut")}
           </p> :
           ReasonReact.null}
```

This is a [ternary](https://reasonml.github.io/reason-react/docs/en/ternary-shortcut#docsNav) function which first checks if the user is logged in or out. If the user, is logged in, the show the dom element with the signout button on it. If not, dont show it. That is the [ReasonReact.null](https://reasonml.github.io/reason-react/docs/en/render#docsNav) part.

Inside the `onClick` key are listing for the click `_event` which we preceed with an underscore to let the compiler know that we won't be using the event, we just want to know that it happened. Then when it happened, call our `signOut` function. Note the signature:

```reason
signOut(amplify, ())
```

We definded it as taking the `amplify` type and then calling unit. It doesn't take and params. Just tells Amplify to sign out anyone who is signed in right now. Then we go to sign the person out on this machine. Our sign in code, we have discussed yet, save a token to local storage. Apparently, when we call `logout` from the `auth.js` we care getting rid of the token. We almost don't have to care. We just give the function what it wants and let it go to work. In this case, it wants a call back to check for errors.

**********
Sidenote: I went to go get you the api for [Js.Exn](https://bucklescript.github.io/bucklescript/api/Js.Exn.html) and ran into the documentation for the Js style try on the bucklescript docs. That, my friends, is how you build the knowledge. I am not even going to write this post pretending I knew that. [Here](https://bucklescript.github.io/docs/en/exceptions#docsNav) it is, we have learned it at the same time.
***********

So the callback calls `navigate` and lets gives any Js erros using the `Js.Exn.Error` module from bucklescript. Thanks, [@bobzhang1988](https://twitter.com/bobzhang1988).

So know we are calling `auth.js` from our reason file which is calling back out to javascript files. **Interop Madness** people. You are not ready!!!

Here is what the working code looks like:

```reason
// Header.re
type amplify;
[@bs.module "@aws-amplify/auth/lib"] external amplify: amplify = "default";
[@bs.send] external signOut: (amplify, unit) => unit = "signOut";

/* this gets the `push` method off of the gatsby dep and works, so does the reach/router which mirros the orignal js. */
[@bs.module "gatsby"] external push: string => unit = "push";
[@bs.module "@reach/router"] external navigate: string => unit = "navigate";
[@bs.module "../utils/auth"] external logout: ('a, unit) => unit = "logout";
[@bs.module "../utils/auth"] external isLoggedIn: unit => bool = "isLoggedIn";

module Styles = {
  let container =
    ReactDOMRe.Style.make(
      ~background="rebeccapurple",
      ~marginBottom="1.45rem",
      (),
    );
  let headerContainer =
    ReactDOMRe.Style.make(
      ~margin="0 auto",
      ~maxWidth="960px",
      ~padding="1.45rem 1.0875rem",
      (),
    );
  let headerTitle =
    ReactDOMRe.Style.make(~color="white", ~textDecoration="none", ());

  let link =
    ReactDOMRe.Style.make(
      ~color="white",
      ~textDecoration="underline",
      ~cursor="pointer",
      (),
    );
};
let component = ReasonReact.statelessComponent("Header");

let make = (~siteTitle: string, _children) => {
  ...component,
  render: _self => {
    // idea from here: https://github.com/xodio/xod/blob/ff7cd119660afbf4b0ea62dddff55f5087d8f150/packages/xod-tabtest/src/Tabtest.re#L87-L90
    let callback =
      try (() => navigate("/app/login")) {
      | Js.Exn.Error(e) => (() => Js.log2("error", e))
      };
    <div style=Styles.container>
      <div style=Styles.headerContainer>
        <h1 style={ReactDOMRe.Style.make(~margin="0", ())}>
          <GatsbyLink to_="/" style=Styles.headerTitle>
            {ReasonReact.string(siteTitle)}
          </GatsbyLink>
        </h1>
        {isLoggedIn() ?
           <p
             style=Styles.link
             onClick={_event =>
               signOut(amplify, ()) |> (() => logout(callback, ()))
             }>
             {ReasonReact.string("SignOut")}
           </p> :
           ReasonReact.null}
      </div>
    </div>;
  },
};

[@bs.deriving abstract]
type jsProps = {siteTitle: string};
// https://reasonml.github.io/reason-react/docs/en/interop#reactjs-using-reasonreact
let default =
  ReasonReact.wrapReasonForJs(~component, jsProps =>
    make(~siteTitle=jsProps->siteTitleGet, [||])
  );
```

Go back into `src/layout.js` and replace the `Header` import. Change:

```js
import Header from './Header'
```

to:

```js
import Header from './Helmet.bs'
```

Run `gatsby develop` to refresh if it didn't. Rejoice!

Incidentally, as most of you know, there are plenty of ways to do this. I am just doing piecemiel so you can see how easy it is without making huge changes. By the end we will have refactored to get the full power of reasonml.

I was going to try to re-create `Helmet.js` in reason just to systematically go through this with you all but it got a bit convuleted. I went to get some help on the [discord](https://discordapp.com/channels/235176658175262720/235176658175262720) which I want to [invite](https://discord.gg/vt3FzA3) you to join. Important point on community help and mental health. I have rules to keep my spirit up when I get frustrated. In relevant part, if you haven't asked someone the question, you are the @$$hole in the room. So go to <www.stackoverflow.com> or <www.reason.chat> or Reason's Discord channel and ask the damned question. If it's an important problem, get the answer on stackoverflow or reason.chat because it will be easier to find later. I don't know how many times I have looked something up and come up on my own previous question or even answer to someone and found the answer while doing an internet or github search. Sharing helps everyone.