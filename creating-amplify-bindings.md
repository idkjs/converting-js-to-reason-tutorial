# Creating AWS Amplify Binding for our IndexPage

We already have `aws-amplify` as a dependency in our project so we need to create what is called a binding with [bucklescript](https://bucklescript.github.io/docs/en/import-export#export) to get it to work in our reason code. Let me just say, this was a hard thing to get my head around and I can't say I have [grokd](https://en.wikipedia.org/wiki/Grok) it yet but what I am about to show you works. To keep with the structure of the original js project, lets a create a file call `Amplify.re` in the `/utils` directory.

In `Amplify.re`, we first need to get access to the `aws-amplify` library via bucklescript and give it a name that we can call in reason with the bucklescript `external` key word.

```reason
type amp;
[@bs.module "@aws-amplify/auth/lib"] external amplify: amp = "default";
```
Here we have created what is called an `opaque type` called `amp` that we will use to to type our `aws-amplify` function. We use `"default"` to say we want to use whatever is being exported as default from that installed npm dependency. In this case the default export in the `@aws-amplify/auth/lib` package is in your node_modules at this path `node_modules/@aws-amplify/auth/lib/index.js`. Whether you know it or not, this is what you have been using when you import a module via npm when you run `npm install`. I don't remember how I found the location but the general idea is to find out where your javascript function that you want to bind to exists. That file is is actually not in the git hub repo which doesn't have a tag for the version of `aws-amplify` that we are using so I can't even point you to it but I did not want to update the dependency for fear of breaking the rest of the project. This is the `Auth` javascript in the current version of the [library](https://github.com/aws-amplify/amplify-js/blob/master/packages/auth/src/Auth.ts).

Ok, so, we then need to get access to our `aws-exports.js` file that was created for us when we ran `amplify init` and `amplify push`. This is how we do it:

```reason
type config;
[@bs.module "../aws-exports.js"] external awsConfig: config = "default";
```

The `aws-exports.js` file exports `awsmobile` by default in the last line with reads:

```js
export default awsmobile;
```

So above we now have a function called `awsConfig` that we can call that will get us our values from `aws-exports.js` and let us connect to our AWS service.


The `amplify` library has a method called `configure` that we want to be able to call. To get access to it, we can set up a binding to call `configure` on our `amplify` binding. Because we created `amplify` as a type that just identifies the library, we can use `[@bs.send]` from bucklescript's [bs.send](https://bucklescript.github.io/docs/en/function#object-method) to call a method on `amplify`. This is what it looks like:

```reason
type configure;
[@bs.send] external configure: (amp, config) => configure = "configure";
```

Her we define `configure` to take a type `amp` and a type `config` the return a type `configure` which calls `configure` on `amplify`. I don't even know if I said that correctly. Basically, when we call `configure` we have to be sure to pass it the amplify library, typed as `amp` and `aws-exports` typed as `config`, which will then take the variable and call `configure`.

Finally we recreate our `Amplify.configure()` function from JS by creating a function called `configure` (could have chosed something less confusing I guess because it doesnt matter), then set it value to the result of calling our `external configure` function passing it the required `amplify` and `awsConfig` parameters.

```reason
let configure = configure(amplify, awsConfig);
```

This is what the whole thing looks like:

```reason
/* Amplify.re */
type amp;
[@bs.module "@aws-amplify/auth/lib"] external amplify: amp = "default";

type config;
[@bs.module "../aws-exports.js"] external awsConfig: config = "default";

type configure;
[@bs.send] external configure: (amp, config) => configure = "configure";

/* let amplify = amplify; */
let configure = configure(amplify, awsConfig);
```

We then go into our `indexPage.re` and call configure from our `Amplify.re` module at the top somewhere so that the entire project has access to the our Aws Amplify service.

Add this to `indexPage.re`.

```reason
Amplify.configure |> Js.log;
```

Run `gatsby develop` and open the terminal in the browser. you should see the output from `Js.log` showing us that we got access to Aws. You might think that it just exporting the variables but if you look closely, you will see some json keys in there that didn't come from `aws-exports.js` but rather from Aws Amplify's response.

Still dont believe its working? Try signing in with some random credentials. Remember that I haven't created a user yet. Even if you have, just sign in with some random credentials by clicking on the `SignIn` link to go to the SignUp page and enter whatever. I entered `someguy` as the `username` and `somePWD{0}` as the password. This is what showed on the screen:

![badSignin.png](./images/badSignin.png)

```sh
errorMessage: {
 "code": "UserNotFoundException",
 "name": "UserNotFoundException",
 "message": "User does not exist."
}
AWS Cognito User Pool documentation.
```

Think about what is going on here. We haven't converted the `SignUp.js` page. The `Amplify.configure` is being used from our ReasonML file in the rest of the projects, javascript files which we linked to in our `indexPage.re` page. This, my friends, is what they call **INTEROP**!

The next thing to do is duplicate the `Layout.js` component which was imported into the original `index.js` and I have no idea how we are going to do that...