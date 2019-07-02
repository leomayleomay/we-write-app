let isActive: ref(bool) = ref(false);

let register = () => {
  isActive := true;
};

let unregister = () => {
  isActive := false;
};

[@bs.deriving abstract]
type window = {
  mutable onbeforeunload:
    (. {. [@bs.set] "returnValue": string}) => Js.Nullable.t(string),
};

[@bs.val] external window: window = "";

module Provider = {
  [@react.component]
  let make = () => {
    React.useEffect0(() => {
      window->onbeforeunloadSet((. event) =>
        switch (isActive^) {
        | false => Js.Nullable.null
        | true =>
          event##returnValue #= "";
          Js.Nullable.null;
        }
      );

      None;
    });

    React.null;
  };
};