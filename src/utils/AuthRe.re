module Env = {
  [@bs.val] external isBrowser: bool = "process.browser";
  type platform =
    | Browser
    | Node;

  let getPlatform = () => isBrowser ? Browser : Node;
};

let saveIt = (identifier, value) =>
  switch (Env.getPlatform()) {
  | Browser =>
    Dom.Storage.(localStorage |> setItem(identifier, value));
    ();
  | _ => ()
  };

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
/* reset store values to logout */
let signOut = () =>
  store(~username="", ~email="", ~phone_number="", ~phone_number_verified="");

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
let isLoggedIn =
  switch (getStore("gatsbyUser")) {
  | Some(storeValues) =>
    try (Js.Json.parseExn(storeValues)->Obj.magic) {
    | exn =>
      Js.log2("Failed to parse store", exn);
      true;
    }
  | None => false
  };

let saveStore = (): unit => {
  saveIt("gatsbyUser", Js.Json.stringify(store->Obj.magic));
};