[@bs.module "react-diff-view"]
external parseDiff: string => array(Js.t('a)) = "";

module Diff = {
  [@bs.module "react-diff-view"] [@react.component]
  external make:
    (
      ~key: string,
      ~viewType: string,
      ~diffType: string,
      ~hunks: array(Js.t('a)),
      ~children: array(Js.t('a)) => React.element
    ) =>
    React.element =
    "Diff";
};

module Hunk = {
  [@bs.module "react-diff-view"] [@react.component]
  external make: (~key: string, ~hunk: Js.t('a)) => React.element = "Hunk";
};