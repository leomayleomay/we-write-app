module Index = {
  let repoFiles = (data: Js.t('a)) => {
    Belt.Option.(
      data##repository
      ->flatMap(repo => repo##obj)
      ->flatMap(obj =>
          switch (obj) {
          | `Tree(tree) => tree##entries
          | _ => None
          }
        )
    );
  };

  let repoNameWithOwner = (data: Js.t('a)) => {
    Belt.Option.(data##repository->map(repo => repo##nameWithOwner)->getExn);
  };

  [@react.component]
  let make = (~owner, ~name) => {
    let repoFilesQuery = Gql_Schema.RepoFiles.make(~owner, ~name, ());
    <Gql_Query.RepoFiles
      variables=repoFilesQuery##variables fetchPolicy="network-only">
      ...{({result}) =>
        switch (result) {
        | Loading =>
          <progress className="progress is-large is-info" max="100" />
        | Error(error) =>
          <div className="notification is-danger">
            {error##message |> React.string}
          </div>
        | Data(repo) =>
          <UI_Repo_Details repo>
            <div className="tabs is-boxed">
              <ul>
                <li className="is-active"> <a> "Files"->React.string </a> </li>
                <li>
                  <a
                    onClick={_ =>
                      ReasonReact.Router.push({j|/$owner/$name/pulls|j})
                    }>
                    "Pull Requests"->React.string
                  </a>
                </li>
              </ul>
            </div>
            <nav className="level">
              <div className="level-left">
                <p className="level-item">
                  <a
                    className="button is-link"
                    onClick={_ =>
                      ReasonReact.Router.push(
                        "/" ++ repo->repoNameWithOwner ++ "/new/master",
                      )
                    }>
                    <span className="icon">
                      <i className="fas fa-plus" ariaHidden=true />
                    </span>
                    <span> "New File"->React.string </span>
                  </a>
                </p>
              </div>
            </nav>
            {repo
             ->repoFiles
             ->Belt.Option.mapWithDefault(React.null, files =>
                 <table className="table is-striped is-bordered is-fullwidth">
                   <tbody>
                     {files
                      |> Array.map(file =>
                           <tr>
                             <td>
                               <a
                                 onClick={_ =>
                                   ReasonReact.Router.push(
                                     "/"
                                     ++ repo->repoNameWithOwner
                                     ++ "/blob/master/"
                                     ++
                                     file##name,
                                   )
                                 }>
                                 {file##name->React.string}
                               </a>
                             </td>
                           </tr>
                         )
                      |> React.array}
                   </tbody>
                 </table>
               )}
          </UI_Repo_Details>
        }
      }
    </Gql_Query.RepoFiles>;
  };
};

module Show = {
  let content = (data: Js.t('a)) => {
    Belt.Option.(
      data##repository
      ->flatMap(repo => repo##obj)
      ->flatMap(obj =>
          switch (obj) {
          | `Blob(blob) => blob##text
          | _ => None
          }
        )
      ->getWithDefault("")
    );
  };
  [@react.component]
  let make = (~owner: string, ~name: string, ~path: string) => {
    let blobQuery =
      Gql_Schema.Blob.make(
        ~owner,
        ~name,
        ~expression="master:" ++ Js.Global.decodeURI(path),
        (),
      );
    <Gql_Query.Blob variables=blobQuery##variables fetchPolicy="network-only">
      ...{({result}) =>
        switch (result) {
        | Loading =>
          <progress className="progress is-large is-info" max="100" />
        | Error(error) =>
          <div className="notification is-danger">
            {error##message->React.string}
          </div>
        | Data(data) =>
          <UI_Repo_Details repo=data>
            <div className="card is-shadowless">
              <header className="card-header">
                <div className="card-header-title">
                  {path->Js.Global.decodeURI->React.string}
                </div>
                <div className="card-header-icon">
                  <a
                    className="button is-link"
                    onClick={_ =>
                      ReasonReact.Router.push(
                        {j|/$owner/$name/edit/master/$path|j},
                      )
                    }>
                    <span className="icon">
                      <i className="fas fa-edit" ariaHidden=true />
                    </span>
                    <span> "Edit"->React.string </span>
                  </a>
                </div>
                <div className="card-header-icon">
                  <a
                    className="button is-danger"
                    onClick={_ =>
                      ReasonReact.Router.push(
                        {j|/$owner/$name/delete/master/$path|j},
                      )
                    }>
                    <span className="icon">
                      <i className="fas fa-trash-alt" ariaHidden=true />
                    </span>
                    <span> "Delete"->React.string </span>
                  </a>
                </div>
              </header>
              <div className="card-content">
                <div className="content is-medium">
                  <p> {data->content->React.string} </p>
                </div>
              </div>
            </div>
          </UI_Repo_Details>
        }
      }
    </Gql_Query.Blob>;
  };
};

module NewForm = {
  module NewFilePath =
    LocalStorage.Operation({
      let key = "we-write-app:new-file-path";
    });

  module NewFileMessage =
    LocalStorage.Operation({
      let key = "we-write-app:new-file-message";
    });

  module NewFileContent =
    LocalStorage.Operation({
      let key = "we-write-app:new-file-content";
    });

  type state = {
    path: string,
    message: string,
    content: string,
    remoteData: RemoteData.t(unit, unit, string),
  };

  type action =
    | UpdatePath(string)
    | UpdateMessage(string)
    | UpdateContent(string)
    | Submit(string, string)
    | HandleResponse(string, string)
    | HandleFailure(string);

  [@react.component]
  let make = (~owner: string, ~name: string, ~isPrivate: bool) => {
    let currentUser = React.useContext(Context.CurrentUser.user);
    let currentUserLogin =
      currentUser->Context.CurrentUser.login->Belt.Option.getExn;

    let (state, send) =
      ReactUpdate.useReducer(
        {
          path: NewFilePath.get()->Belt.Option.getWithDefault(""),
          message: NewFileMessage.get()->Belt.Option.getWithDefault(""),
          content: NewFileContent.get()->Belt.Option.getWithDefault(""),
          remoteData: RemoteData.NotAsked,
        },
        (action, state) =>
        switch (action) {
        | UpdatePath(path) =>
          UpdateWithSideEffects(
            {...state, path},
            _ => {
              path->NewFilePath.set->ignore;
              None;
            },
          )
        | UpdateMessage(message) =>
          UpdateWithSideEffects(
            {...state, message},
            _ => {
              message->NewFileMessage.set->ignore;
              None;
            },
          )
        | UpdateContent(content) =>
          UpdateWithSideEffects(
            {...state, content},
            _ => {
              content->NewFileContent.set->ignore;
              None;
            },
          )
        | Submit(newOwner, newBranch) =>
          UpdateWithSideEffects(
            {...state, remoteData: Loading()},
            ({send}) => {
              Js.Promise.(
                Github_API_Client.createFile(
                  ~repo={j|$newOwner/$name|j},
                  ~branch=newBranch,
                  ~path=state.path,
                  ~message=state.message,
                  ~content=state.content,
                )
                |> then_(_response => {
                     NewFilePath.remove();
                     NewFileMessage.remove();
                     NewFileContent.remove();
                     send(HandleResponse(newOwner, newBranch));
                     resolve();
                   })
                |> catch(error =>
                     send(HandleFailure(Js.String.make(error))) |> resolve
                   )
                |> ignore
              );

              None;
            },
          )
        | HandleResponse(newOwner, newBranch) =>
          SideEffects(
            _self => {
              if (newBranch == "master") {
                ReasonReact.Router.push({j|/$owner/$name|j});
              } else if (newOwner == owner) {
                ReasonReact.Router.push(
                  {j|/$owner/$name/compare/master/$newBranch|j},
                );
              } else {
                ReasonReact.Router.push(
                  {j|/$owner/$name/compare/master/$newOwner:$newBranch|j},
                );
              };
              None;
            },
          )
        | HandleFailure(error) =>
          Update({...state, remoteData: Failure(error)})
        }
      );

    React.useEffect3(
      () => {
        String.length(state.path) > 0
        || String.length(state.message) > 0
        || String.length(state.content) > 0
          ? OnBeforeUnload.register() : OnBeforeUnload.unregister();

        Some(() => OnBeforeUnload.unregister());
      },
      (state.path, state.message, state.content),
    );

    switch (state.remoteData) {
    | NotAsked =>
      <MaybeFork
        owner
        name
        onSuccess={newBranch => send(Submit(currentUserLogin, newBranch))}>
        ...{(~viewerPermission, ~createFork, ~createBranch) =>
          <>
            {viewerPermission == `READ
               ? <article className="message is-info">
                   <div className="message-header">
                     <p> "Notice"->React.string </p>
                   </div>
                   <div className="message-body">
                     {j|You’re creating a file in a project you don’t have write access to. Submitting a change will create the file in a new branch in your fork $currentUserLogin/$name, so you can send a pull request.|j}
                     ->React.string
                   </div>
                 </article>
               : React.null}
            <UI_Control.TextInput
              label="Name"
              value={state.path}
              onChange={path => send(UpdatePath(path))}
            />
            <UI_Control.TextArea
              label="Content"
              rows=20
              value={state.content}
              onChange={content => send(UpdateContent(content))}
            />
            <UI_Control.TextInput
              label="Describe your changes"
              value={state.message}
              onChange={message => send(UpdateMessage(message))}
            />
            <UI_Control.Button
              text="Create"
              onClick={_ =>
                switch (viewerPermission) {
                | `ADMIN
                | `WRITE when isPrivate => send(Submit(owner, "master"))
                | `ADMIN
                | `WRITE => createBranch()
                | `READ => createFork()
                }
              }
            />
          </>
        }
      </MaybeFork>
    | Loading () =>
      <progress className="progress is-large is-info" max="100" />
    | Success () => React.null
    | Failure(error) =>
      <div className="notification is-danger"> error->React.string </div>
    };
  };
};

module New = {
  let isPrivate = (data: Js.t('a)) => {
    Belt.Option.(data##repository->map(repo => repo##isPrivate)->getExn);
  };

  [@react.component]
  let make = (~owner: string, ~name: string) => {
    let repoQuery = Gql_Schema.Repo.make(~owner, ~name, ());
    <Gql_Query.Repo variables=repoQuery##variables>
      ...{({result}) =>
        switch (result) {
        | Loading =>
          <progress className="progress is-large is-info" max="100" />
        | Error(error) =>
          <div className="notification is-danger">
            {error##message->React.string}
          </div>
        | Data(data) =>
          <UI_Repo_Details repo=data>
            <NewForm owner name isPrivate={data->isPrivate} />
          </UI_Repo_Details>
        }
      }
    </Gql_Query.Repo>;
  };
};

module EditForm = {
  type state = {
    path: string,
    message: string,
    content: string,
    remoteData: RemoteData.t(unit, unit, string),
  };

  type action =
    | UpdatePath(string)
    | UpdateMessage(string)
    | UpdateContent(string)
    | Submit(string, string)
    | HandleResponse(string, string)
    | HandleFailure(string);

  [@react.component]
  let make =
      (
        ~owner: string,
        ~name: string,
        ~sha: string,
        ~path: string,
        ~content: string,
        ~isPrivate: bool,
      ) => {
    let currentUser = React.useContext(Context.CurrentUser.user);
    let currentUserLogin =
      currentUser->Context.CurrentUser.login->Belt.Option.getExn;

    let (state, send) =
      ReactUpdate.useReducer(
        {
          path: Js.Global.decodeURI(path),
          message: "",
          content,
          remoteData: RemoteData.NotAsked,
        },
        (action, state) =>
        switch (action) {
        | UpdatePath(path) => Update({...state, path})
        | UpdateMessage(message) => Update({...state, message})
        | UpdateContent(content) => Update({...state, content})
        | Submit(newOwner, newBranch) =>
          UpdateWithSideEffects(
            {...state, remoteData: Loading()},
            ({send}) => {
              Js.Promise.(
                Github_API_Client.updateFile(
                  ~repo={j|$newOwner/$name|j},
                  ~branch=newBranch,
                  ~sha,
                  ~path=state.path,
                  ~message=state.message,
                  ~content=state.content,
                )
                |> then_(_ =>
                     send(HandleResponse(newOwner, newBranch)) |> resolve
                   )
                |> catch(error =>
                     send(HandleFailure(Js.String.make(error))) |> resolve
                   )
                |> ignore
              );
              None;
            },
          )
        | HandleResponse(newOwner, newBranch) =>
          SideEffects(
            _self => {
              if (newBranch == "master") {
                ReasonReact.Router.push({j|/$owner/$name/|j});
              } else if (newOwner == owner) {
                ReasonReact.Router.push(
                  {j|/$owner/$name/compare/master/$newBranch|j},
                );
              } else {
                ReasonReact.Router.push(
                  {j|/$owner/$name/compare/master/$newOwner:$newBranch|j},
                );
              };

              None;
            },
          )
        | HandleFailure(error) =>
          Update({...state, remoteData: Failure(error)})
        }
      );

    React.useEffect3(
      () => {
        String.length(state.path) > 0
        || String.length(state.message) > 0
        || String.length(state.content) > 0
          ? OnBeforeUnload.register() : OnBeforeUnload.unregister();

        Some(() => OnBeforeUnload.unregister());
      },
      (state.path, state.message, state.content),
    );

    switch (state.remoteData) {
    | NotAsked =>
      <MaybeFork
        owner
        name
        onSuccess={newBranch =>
          send(
            Submit(
              currentUser->Context.CurrentUser.login->Belt.Option.getExn,
              newBranch,
            ),
          )
        }>
        ...{(~viewerPermission, ~createFork, ~createBranch) =>
          <>
            {viewerPermission == `READ
               ? <article className="message is-info">
                   <div className="message-header">
                     <p> "Notice"->React.string </p>
                   </div>
                   <div className="message-body">
                     {j|You’re editing a file in a project you don’t have write access to. Submitting a change will update the file in a new branch in your fork $currentUserLogin/$name, so you can send a pull request.|j}
                     ->React.string
                   </div>
                 </article>
               : React.null}
            <UI_Control.TextInput
              label="Name"
              value={state.path}
              onChange={path => send(UpdatePath(path))}
            />
            <UI_Control.TextArea
              label="Content"
              rows=20
              value={state.content}
              onChange={content => send(UpdateContent(content))}
            />
            <UI_Control.TextInput
              label="Describe your changes"
              value={state.message}
              onChange={message => send(UpdateMessage(message))}
            />
            <UI_Control.Button
              text="Update"
              onClick={_ =>
                switch (viewerPermission) {
                | `ADMIN
                | `WRITE when isPrivate => send(Submit(owner, "master"))
                | `ADMIN
                | `WRITE => createBranch()
                | `READ => createFork()
                }
              }
            />
          </>
        }
      </MaybeFork>
    | Loading () =>
      <progress className="progress is-large is-info" max="100" />
    | Success () => React.null
    | Failure(error) =>
      <div className="notification is-danger"> error->React.string </div>
    };
  };
};

module Edit = {
  let sha = (data: Js.t('a)) => {
    Belt.Option.(
      data##repository
      ->flatMap(repo => repo##obj)
      ->flatMap(obj =>
          switch (obj) {
          | `Blob(blob) => Js.Json.decodeString(blob##oid)
          | _ => None
          }
        )
      ->getExn
    );
  };

  let content = (data: Js.t('a)) => {
    Belt.Option.(
      data##repository
      ->flatMap(repo => repo##obj)
      ->flatMap(obj =>
          switch (obj) {
          | `Blob(blob) => blob##text
          | _ => None
          }
        )
      ->getWithDefault("")
    );
  };

  let isPrivate = (data: Js.t('a)) => {
    Belt.Option.(data##repository->map(repo => repo##isPrivate)->getExn);
  };

  [@react.component]
  let make = (~owner: string, ~name: string, ~path: string) => {
    let blobQuery =
      Gql_Schema.Blob.make(
        ~owner,
        ~name,
        ~expression="master:" ++ Js.Global.decodeURI(path),
        (),
      );
    <Gql_Query.Blob variables=blobQuery##variables>
      ...{({result}) =>
        switch (result) {
        | Loading =>
          <progress className="progress is-large is-info" max="100" />
        | Error(error) =>
          <div className="notification is-danger">
            {error##message->React.string}
          </div>
        | Data(data) =>
          <UI_Repo_Details repo=data>
            <EditForm
              owner
              name
              sha={data->sha}
              path
              content={data->content}
              isPrivate={data->isPrivate}
            />
          </UI_Repo_Details>
        }
      }
    </Gql_Query.Blob>;
  };
};

module DeleteForm = {
  type state = {
    message: string,
    remoteData: RemoteData.t(unit, unit, string),
  };
  type action =
    | UpdateMessage(string)
    | Submit(string, string)
    | HandleResponse(string, string)
    | HandleFailure(string);
  [@react.component]
  let make =
      (
        ~owner: string,
        ~name: string,
        ~sha: string,
        ~path: string,
        ~content: string,
        ~isPrivate: bool,
      ) => {
    let currentUser = React.useContext(Context.CurrentUser.user);
    let currentUserLogin =
      currentUser->Context.CurrentUser.login->Belt.Option.getExn;
    let (state, send) =
      ReactUpdate.useReducer(
        {message: "", remoteData: RemoteData.NotAsked}, (action, state) =>
        switch (action) {
        | UpdateMessage(message) => Update({...state, message})
        | Submit(newOwner, newBranch) =>
          UpdateWithSideEffects(
            {...state, remoteData: Loading()},
            ({send}) => {
              Js.Promise.(
                Github_API_Client.deleteFile(
                  ~repo={j|$newOwner/$name|j},
                  ~branch=newBranch,
                  ~sha,
                  ~path,
                  ~message=state.message,
                )
                |> then_(_response =>
                     send(HandleResponse(newOwner, newBranch)) |> resolve
                   )
                |> catch(error =>
                     send(HandleFailure(Js.String.make(error))) |> resolve
                   )
                |> ignore
              );
              None;
            },
          )

        | HandleResponse(newOwner, newBranch) =>
          SideEffects(
            _self => {
              if (newBranch == "master") {
                ReasonReact.Router.push({j|/$owner/$name|j});
              } else if (newOwner == owner) {
                ReasonReact.Router.push(
                  {j|/$owner/$name/compare/master/$newBranch|j},
                );
              } else {
                ReasonReact.Router.push(
                  {j|/$owner/$name/compare/master/$newOwner:$newBranch|j},
                );
              };

              None;
            },
          )
        | HandleFailure(error) =>
          Update({...state, remoteData: Failure(error)})
        }
      );
    switch (state.remoteData) {
    | NotAsked =>
      <MaybeFork
        owner
        name
        onSuccess={newBranch =>
          send(
            Submit(
              currentUser->Context.CurrentUser.login->Belt.Option.getExn,
              newBranch,
            ),
          )
        }>
        ...{(~viewerPermission, ~createFork, ~createBranch) =>
          <>
            {viewerPermission == `READ
               ? <article className="message is-info">
                   <div className="message-header">
                     <p> "Notice"->React.string </p>
                   </div>
                   <div className="message-body">
                     {j|You’re deleting a file in a project you don’t have write access to. Submitting a change will update the file in a new branch in your fork $currentUserLogin/$name, so you can send a pull request.|j}
                     ->React.string
                   </div>
                 </article>
               : React.null}
            <div className="content is-medium">
              <p> content->React.string </p>
            </div>
            <UI_Control.TextInput
              label="Describe your changes"
              value={state.message}
              onChange={message => send(UpdateMessage(message))}
            />
            <UI_Control.Button
              text="Delete"
              onClick={_ =>
                switch (viewerPermission) {
                | `ADMIN
                | `WRITE when isPrivate => send(Submit(owner, "master"))
                | `ADMIN
                | `WRITE => createBranch()
                | `READ => createFork()
                }
              }
            />
          </>
        }
      </MaybeFork>
    | Loading () =>
      <progress className="progress is-large is-info" max="100" />
    | Success () => React.null
    | Failure(error) =>
      <div className="notification is-danger"> error->React.string </div>
    };
  };
};

module Delete = {
  let sha = (data: Js.t('a)) => {
    Belt.Option.(
      data##repository
      ->flatMap(repo => repo##obj)
      ->flatMap(obj =>
          switch (obj) {
          | `Blob(blob) => Js.Json.decodeString(blob##oid)
          | _ => None
          }
        )
      ->getExn
    );
  };

  let content = (data: Js.t('a)) => {
    Belt.Option.(
      data##repository
      ->flatMap(repo => repo##obj)
      ->flatMap(obj =>
          switch (obj) {
          | `Blob(blob) => blob##text
          | _ => None
          }
        )
      ->getWithDefault("")
    );
  };

  let isPrivate = (data: Js.t('a)) => {
    Belt.Option.(data##repository->map(repo => repo##isPrivate)->getExn);
  };

  [@react.component]
  let make = (~owner: string, ~name: string, ~path: string) => {
    let blobQuery =
      Gql_Schema.Blob.make(
        ~owner,
        ~name,
        ~expression="master:" ++ Js.Global.decodeURI(path),
        (),
      );
    <Gql_Query.Blob variables=blobQuery##variables>
      ...{({result}) =>
        switch (result) {
        | Loading =>
          <progress className="progress is-large is-info" max="100" />
        | Error(error) =>
          <div className="notification is-danger">
            {error##message->React.string}
          </div>
        | Data(data) =>
          <UI_Repo_Details repo=data>
            <DeleteForm
              owner
              name
              sha={data->sha}
              path
              content={data->content}
              isPrivate={data->isPrivate}
            />
          </UI_Repo_Details>
        }
      }
    </Gql_Query.Blob>;
  };
};