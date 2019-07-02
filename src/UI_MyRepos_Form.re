type response = {. "full_name": string};

type state = {
  name: string,
  description: string,
  remoteData: RemoteData.t(unit, unit, string),
};

type action =
  | UpdateName(string)
  | UpdateDescription(string)
  | Submit
  | HandleResponse(response)
  | HandleFailure(string);

[@react.component]
let make = (~repo: option(Js.t('a)), ~onSuccess: unit => unit) => {
  let createRepo = (~params, ~onSuccess, ~onFailure) => {
    Js.Promise.(
      Github_API_Client.createRepo(~params)
      |> then_(response => onSuccess(response##data) |> resolve)
      |> catch(error => onFailure(Js.String.make(error)) |> resolve)
      |> ignore
    );
  };

  let updateRepo = (~repo, ~params, ~onSuccess, ~onFailure) => {
    Js.Promise.(
      Github_API_Client.updateRepo(~repo=repo##nameWithOwner, ~params)
      |> then_(response => onSuccess(response##data) |> resolve)
      |> catch(error => onFailure(Js.String.make(error)) |> resolve)
      |> ignore
    );
  };

  let (state, send) =
    ReactUpdate.useReducer(
      {
        name: repo->Belt.Option.mapWithDefault("", repo => repo##name),
        description:
          Belt.Option.(
            repo->flatMap(repo => repo##description)->getWithDefault("")
          ),
        remoteData: RemoteData.NotAsked,
      },
      (action, state) =>
      switch (action) {
      | UpdateName(name) => Update({...state, name})
      | UpdateDescription(description) => Update({...state, description})
      | Submit =>
        UpdateWithSideEffects(
          {...state, remoteData: Loading()},
          ({send}) => {
            switch (repo) {
            | Some(repo) =>
              updateRepo(
                ~repo,
                ~params={
                  "name": state.name,
                  "description": state.description,
                },
                ~onSuccess=response => send(HandleResponse(response)),
                ~onFailure=error => send(HandleFailure(error)),
              )
            | None =>
              createRepo(
                ~params={
                  "name": state.name,
                  "description": state.description,
                  "private": true,
                  "auto_init": true,
                },
                ~onSuccess=response => send(HandleResponse(response)),
                ~onFailure=error => send(HandleFailure(error)),
              )
            };
            None;
          },
        )
      | HandleResponse(response) =>
        onSuccess();
        SideEffects(
          _self => {
            ReasonReact.Router.push("/" ++ response##full_name);
            None;
          },
        );
      | HandleFailure(error) =>
        Update({...state, remoteData: Failure(error)})
      }
    );

  let viewForm = () => {
    <>
      <UI_Control.TextInput
        label="Name"
        value={state.name}
        onChange={name => send(UpdateName(name))}
      />
      <UI_Control.TextInput
        label="Description"
        value={state.description}
        onChange={description => send(UpdateDescription(description))}
      />
      <UI_Control.Button text="Submit" onClick={_ => send(Submit)} />
    </>;
  };

  switch (state.remoteData) {
  | NotAsked => viewForm()
  | Loading () => <progress className="progress is-large is-info" max="100" />
  | Success () => React.null
  | Failure(error) =>
    <>
      <div className="notification is-danger"> {error |> React.string} </div>
      {viewForm()}
    </>
  };
};