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

    // let callback =
    //    try (() => push("/app/login")) {
    //    | Js.Exn.Error(e) => (() => Js.log(e))
    //    };
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