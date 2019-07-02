module Index = {
  let hasPreviousPage = (data: Js.t('a)) => {
    Belt.Option.(
      data##repository
      ->mapWithDefault(false, repo =>
          repo##pullRequests##pageInfo##hasPreviousPage
        )
    );
  };

  let hasNextPage = (data: Js.t('a)) => {
    Belt.Option.(
      data##repository
      ->mapWithDefault(false, repo =>
          repo##pullRequests##pageInfo##hasNextPage
        )
    );
  };

  let startCursor = (data: Js.t('a)) => {
    Belt.Option.(
      data##repository
      ->flatMap(repo => repo##pullRequests##pageInfo##startCursor)
      ->getExn
    );
  };

  let endCursor = (data: Js.t('a)) => {
    Belt.Option.(
      data##repository
      ->flatMap(repo => repo##pullRequests##pageInfo##endCursor)
      ->getExn
    );
  };

  let updateQuery = (prev, next) =>
    Belt.Option.getWithDefault(
      next->ReasonApolloQuery.fetchMoreResultGet,
      prev,
    );

  [@react.component]
  let make = (~owner: string, ~name: string) => {
    let pullRequestsQuery =
      Gql_Schema.PullRequests.make(~owner, ~name, ~first=100, ());

    <Gql_Query.PullRequests variables=pullRequestsQuery##variables>
      ...{({result, fetchMore}) =>
        switch (result) {
        | Loading =>
          <progress className="progress is-large is-info" max="100" />
        | Error(error) =>
          <div className="notification is-danger">
            {error##message |> React.string}
          </div>
        | Data(data) =>
          <UI_Repo_Details repo=data>
            <div className="tabs is-boxed">
              <ul>
                <li>
                  <a
                    onClick={_ =>
                      ReasonReact.Router.push({j|/$owner/$name|j})
                    }>
                    "Files"->React.string
                  </a>
                </li>
                <li className="is-active">
                  <a> "Pull Requests"->React.string </a>
                </li>
              </ul>
            </div>
            {data##repository
             ->Belt.Option.flatMap(repo => repo##pullRequests##nodes)
             ->Belt.Option.mapWithDefault(React.null, pullRequests =>
                 <table className="table is-striped is-bordered is-fullwidth">
                   <tbody>
                     {pullRequests->Belt.Array.keepMap(x => x)
                      |> Array.map(pullRequest =>
                           <tr>
                             <td>
                               {"#"
                                ++ string_of_int(pullRequest##number)
                                |> React.string}
                             </td>
                             <td>
                               <a
                                 onClick={_ =>
                                   ReasonReact.Router.push(
                                     {j|/$owner/$name/pull/|j}
                                     ++ string_of_int(pullRequest##number),
                                   )
                                 }>
                                 {pullRequest##title |> React.string}
                               </a>
                             </td>
                           </tr>
                         )
                      |> React.array}
                   </tbody>
                 </table>
               )}
            <nav
              className="pagination" role="navigation" ariaLabel="pagination">
              {data->hasPreviousPage
                 ? {
                   let morePullRequestsQuery =
                     Gql_Schema.PullRequests.make(
                       ~owner,
                       ~name,
                       ~last=100,
                       ~before=data->startCursor,
                       (),
                     );
                   <a
                     className="pagination-previous"
                     onClick={_ =>
                       fetchMore(
                         ~variables=morePullRequestsQuery##variables,
                         ~updateQuery,
                         (),
                       )
                       |> ignore
                     }>
                     "Previous"->React.string
                   </a>;
                 }
                 : React.null}
              {data->hasNextPage
                 ? {
                   let morePullRequestsQuery =
                     Gql_Schema.PullRequests.make(
                       ~owner,
                       ~name,
                       ~first=100,
                       ~after=data->endCursor,
                       (),
                     );
                   <a
                     className="pagination-next"
                     onClick={_ =>
                       fetchMore(
                         ~variables=morePullRequestsQuery##variables,
                         ~updateQuery,
                         (),
                       )
                       |> ignore
                     }>
                     "Next"->React.string
                   </a>;
                 }
                 : React.null}
            </nav>
          </UI_Repo_Details>
        }
      }
    </Gql_Query.PullRequests>;
  };
};

module Show = {
  let title = (data: Js.t('a)) => {
    Belt.Option.(
      data##repository
      ->flatMap(repo => repo##pullRequest)
      ->map(pr => pr##title)
      ->getExn
    );
  };

  let viewerDidAuthor = (data: Js.t('a)) => {
    Belt.Option.(
      data##repository
      ->flatMap(repo => repo##pullRequest)
      ->mapWithDefault(false, pr => pr##viewerDidAuthor)
    );
  };

  type tab =
    | Comments
    | Reviews
    | Files;

  [@react.component]
  let make = (~owner: string, ~name: string, ~number: int) => {
    let (currentTab, setCurrentTab) = React.useState(() => Comments);

    let pullRequestQuery =
      Gql_Schema.PullRequest.make(~owner, ~name, ~number, ());

    <Gql_Query.PullRequest variables=pullRequestQuery##variables>
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
            <div className="tabs is-boxed">
              <ul>
                <li>
                  <a
                    onClick={_ =>
                      ReasonReact.Router.push({j|/$owner/$name|j})
                    }>
                    "Files"->React.string
                  </a>
                </li>
                <li className="is-active">
                  <a
                    onClick={_ =>
                      ReasonReact.Router.push({j|/$owner/$name/pulls|j})
                    }>
                    "Pull Requests"->React.string
                  </a>
                </li>
              </ul>
            </div>
            <div className="card is-shadowless">
              <header className="card-header">
                <div className="card-header-title">
                  {data->title->React.string}
                </div>
                {data->viewerDidAuthor
                   ? <p className="card-header-icon">
                       <UI_PullRequest_Merge pullRequest=data />
                     </p>
                   : React.null}
              </header>
              <div className="card-content">
                <div className="tabs is-boxed">
                  <ul>
                    <li className={currentTab == Comments ? "is-active" : ""}>
                      <a
                        onClick={_ =>
                          currentTab == Comments
                            ? () : setCurrentTab(_ => Comments)
                        }>
                        "Comments"->React.string
                      </a>
                    </li>
                    <li className={currentTab == Reviews ? "is-active" : ""}>
                      <a
                        onClick={_ =>
                          currentTab == Reviews
                            ? () : setCurrentTab(_ => Reviews)
                        }>
                        "Reviews"->React.string
                      </a>
                    </li>
                    <li className={currentTab == Files ? "is-active" : ""}>
                      <a
                        onClick={_ =>
                          currentTab == Files ? () : setCurrentTab(_ => Files)
                        }>
                        "Files Changed"->React.string
                      </a>
                    </li>
                  </ul>
                </div>
                {switch (currentTab) {
                 | Comments =>
                   <UI_PullRequest_Comments.Index owner name number />
                 | Reviews =>
                   <UI_PullRequest_Reviews.Index owner name number />
                 | Files => <UI_PullRequest_Files.Index owner name number />
                 }}
              </div>
            </div>
          </UI_Repo_Details>
        }
      }
    </Gql_Query.PullRequest>;
  };
};

module New = {
  [@react.component]
  let make = (~owner, ~name, ~baseRefName, ~headRefName) => {
    let (title, setTitle) = React.useState(() => "");
    let (body, setBody) = React.useState(() => "");

    React.useEffect2(
      () => {
        String.length(title) > 0 || String.length(body) > 0
          ? OnBeforeUnload.register() : OnBeforeUnload.unregister();

        Some(() => OnBeforeUnload.unregister());
      },
      (title, body),
    );

    let repoQuery = Gql_Schema.Repo.make(~owner, ~name, ());
    <Gql_Query.Repo variables=repoQuery##variables>
      ...{({result}) =>
        switch (result) {
        | Loading => <div> {"Loading repository!" |> React.string} </div>
        | Error(error) => <div> {error##message |> React.string} </div>
        | Data(data) =>
          <div className="card is-shadowless">
            <header className="card-header">
              <div className="card-header-title">
                "Describe your changes in details"->React.string
              </div>
            </header>
            <div className="card-content">
              <Gql_Mutation.CreatePullRequest>
                ...{(mutate, {result}) =>
                  switch (result) {
                  | NotCalled =>
                    <>
                      <UI_Control.TextInput
                        label="Title"
                        value=title
                        onChange=setTitle
                      />
                      <UI_Control.TextArea
                        label="Body"
                        rows=5
                        value=body
                        onChange=setBody
                      />
                      <UI_Control.Button
                        text="Create pull request"
                        onClick={_ => {
                          let createPullRequest =
                            Gql_Schema.CreatePullRequest.make(
                              ~repositoryId=
                                Belt.Option.(
                                  data##repository
                                  ->map(repo => repo##id)
                                  ->getExn
                                ),
                              ~baseRefName,
                              ~headRefName,
                              ~title,
                              ~body,
                              (),
                            );

                          mutate(~variables=createPullRequest##variables, ())
                          |> ignore;
                        }}
                      />
                    </>
                  | Data(data) =>
                    <Show
                      owner
                      name
                      number=Belt.Option.(
                        data##createPullRequest
                        ->flatMap(createPr => createPr##pullRequest)
                        ->map(pr => pr##number)
                        ->getExn
                      )
                    />
                  | Loading =>
                    <progress
                      className="progress is-large is-info"
                      max="100"
                    />
                  | Error(error) =>
                    <div className="notification is-danger">
                      {error##message->React.string}
                    </div>
                  }
                }
              </Gql_Mutation.CreatePullRequest>
            </div>
          </div>
        }
      }
    </Gql_Query.Repo>;
  };
};