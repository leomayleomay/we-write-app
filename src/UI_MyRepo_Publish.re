type state = RemoteData.t(unit, unit, string);

type action =
  | Publish
  | HandleResponse
  | HandleFailure(string);

[@react.component]
let make = (~repo) => {
  let (state, send) =
    ReactUpdate.useReducer(RemoteData.NotAsked, (action, _state) =>
      switch (action) {
      | Publish =>
        UpdateWithSideEffects(
          Loading(),
          ({send}) => {
            Js.Promise.(
              Github_API_Client.updateRepo(
                ~repo=repo##nameWithOwner,
                ~params={"name": repo##name, "private": false},
              )
              |> then_(_ => send(HandleResponse) |> resolve)
              |> catch(error =>
                   send(HandleFailure(Js.String.make(error))) |> resolve
                 )
              |> ignore
            );
            None;
          },
        )
      | HandleResponse => Update(Success())
      | HandleFailure(error) => Update(Failure(error))
      }
    );

  switch (state) {
  | NotAsked =>
    <a
      onClick={_ => {
        let continue =
          Webapi.Dom.(
            Window.confirm("Do you really want to publish the works?", window)
          );

        if (continue) {
          send(Publish);
        };
      }}>
      {"Publish" |> React.string}
    </a>
  | Loading () => <progress className="progress is-large is-info" max="100" />
  | Success () => React.null
  | Failure(error) =>
    <div className="notification is-danger"> {error |> React.string} </div>
  };
};