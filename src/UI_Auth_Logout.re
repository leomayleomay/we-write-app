[@react.component]
let make = () => {
  React.useEffect0(() => {
    Auth.resetStore();
    Gql_Client.resetStore() |> ignore;
    ReasonReact.Router.push("/");
    None;
  });
  "Logging out..." |> React.string;
};