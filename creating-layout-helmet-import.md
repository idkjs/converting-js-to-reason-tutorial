# Creating Layout.js's Helmet Import - Part 3

![Kublai Kahn](https://raw.githubusercontent.com/idkjs/converting-js-to-reason-tutorial/master/images/medium-header.jpeg)


Create a file in `src/components` called `Helmet.re`.

Add this to it:

```reason
[@bs.module "react-helmet"]
external helmet : ReasonReact.reactClass = "Helmet";

[@bs.deriving abstract]
type metaField = {
  name: string,
  content: string
};

let make = (~title: option(string)=?, ~meta: option(array(metaField))=?, children) =>
  ReasonReact.wrapJsForReason(
    ~reactClass=helmet,
    ~props={
      "title": Js.Nullable.fromOption(title),
      "meta": Js.Nullable.fromOption(meta)
    },
    children
);
```

Go back into `layout.js` and replace the `Helmet` import. Change:

```js
import Helmet from 'react-helmet'
```

to:

```js
import Helmet from './Helmet.bs'
```

Run `gatsby develop` to refresh if it didn't. Rejoice!