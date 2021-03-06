// Generated by BUCKLESCRIPT VERSION 4.0.18, PLEASE EDIT WITH CARE
'use strict';

var Caml_option = require("bs-platform/lib/js/caml_option.js");
var Caml_js_exceptions = require("bs-platform/lib/js/caml_js_exceptions.js");

function getPlatform(param) {
  var match = process.browser;
  if (match) {
    return /* Browser */0;
  } else {
    return /* Node */1;
  }
}

var Env = /* module */[/* getPlatform */getPlatform];

function saveIt(identifier, value) {
  var match = getPlatform(/* () */0);
  if (match) {
    return /* () */0;
  } else {
    localStorage.setItem(identifier, value);
    return /* () */0;
  }
}

function getStore(identifier) {
  var match = getPlatform(/* () */0);
  if (match) {
    return undefined;
  } else {
    return Caml_option.null_to_opt(localStorage.getItem(identifier));
  }
}

function signOut(param) {
  return {
          username: "",
          email: "",
          phone_number: "",
          phone_number_verified: ""
        };
}

var match = getStore("gatsbyUser");

var store;

if (match !== undefined) {
  try {
    store = JSON.parse(match);
  }
  catch (raw_exn){
    var exn = Caml_js_exceptions.internalToOCamlException(raw_exn);
    console.log("Failed to parse store", exn);
    store = {
      username: "",
      email: "",
      phone_number: "",
      phone_number_verified: ""
    };
  }
} else {
  store = {
    username: "",
    email: "",
    phone_number: "",
    phone_number_verified: ""
  };
}

var match$1 = getStore("gatsbyUser");

var isLoggedIn;

if (match$1 !== undefined) {
  try {
    isLoggedIn = JSON.parse(match$1);
  }
  catch (raw_exn$1){
    var exn$1 = Caml_js_exceptions.internalToOCamlException(raw_exn$1);
    console.log("Failed to parse store", exn$1);
    isLoggedIn = true;
  }
} else {
  isLoggedIn = false;
}

function saveStore(param) {
  return saveIt("gatsbyUser", JSON.stringify(store));
}

exports.Env = Env;
exports.saveIt = saveIt;
exports.getStore = getStore;
exports.signOut = signOut;
exports.store = store;
exports.isLoggedIn = isLoggedIn;
exports.saveStore = saveStore;
/* match Not a pure module */
