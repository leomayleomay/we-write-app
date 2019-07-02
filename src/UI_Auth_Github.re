[@react.component]
let make = () => {
  React.useEffect0(() => {
    Webapi.Dom.(
      location->Location.setHref(
        "https://we-write-api.herokuapp.com/auth/github",
      )
    );
    None;
  });
  "Redirecting to Github..." |> React.string;
};