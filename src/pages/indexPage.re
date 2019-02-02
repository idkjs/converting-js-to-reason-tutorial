let component = ReasonReact.statelessComponent("IndexPage");
Amplify.configure;

/* Js.log(Amplify.configure); */
/* create a text element so we dont have to keep writing ReasonReact.string.
So now we can do (text("Hi People")) instead of ReasonReact.string("Hi People")
*/
let text = ReasonReact.string;
let make = _children => {
  ...component,
  render: _self =>
    <div>
      <h1> (text("Hi People")) </h1>
      <p> (text("Welcome to your new Gatsby site.")) </p>
      <p> (text("Welcome to your new Gatsby site with multi-user authentication powered by"))<a href="https://amplify.aws">(text("AWS Amplify"))</a></p>
      <p> (text("Create a new account: "))<GatsbyLink to_="/app/signup">(text("Sign Up"))</GatsbyLink></p>
      <GatsbyLink to_="/app/login"> (text("Sign In"))</GatsbyLink><br />
      <GatsbyLink to_="/app/home"> (text("Home"))</GatsbyLink><br />
      <GatsbyLink to_="/app/profile"> (text("Your Profile"))</GatsbyLink>
    </div>
};
let default =
ReasonReact.wrapReasonForJs(~component, jsProps => make(jsProps##children));