module Index = {
  let pullRequestViewerCanReact = (data: Js.t('a)): bool => {
    Belt.Option.(
      data##repository
      ->flatMap(repo => repo##pullRequest)
      ->mapWithDefault(false, pr => pr##viewerCanReact)
    );
  };

  let pullRequestId = (data: Js.t('a)) => {
    Belt.Option.(
      data##repository
      ->flatMap(repo => repo##pullRequest)
      ->map(pr => pr##id)
      ->getExn
    );
  };

  let pullRequestBodyText = (data: Js.t('a)) => {
    Belt.Option.(
      data##repository
      ->flatMap(repo => repo##pullRequest)
      ->mapWithDefault("", pr => pr##bodyText)
    );
  };

  let pullRequestAuthorLogin = (data: Js.t('a)) => {
    Belt.Option.(
      data##repository
      ->flatMap(repo => repo##pullRequest)
      ->flatMap(pr =>
          switch (pr##author) {
          | Some(`Actor(author)) => Some(author##login)
          | _ => None
          }
        )
    );
  };

  let pullRequestAuthorAvatarUrl = (data: Js.t('a)) => {
    Belt.Option.(
      data##repository
      ->flatMap(repo => repo##pullRequest)
      ->flatMap(pr =>
          switch (pr##author) {
          | Some(`Actor(author)) => Js.Json.decodeString(author##avatarUrl)
          | _ => None
          }
        )
    );
  };

  let pullRequestCreatedAt = (data: Js.t('a)) => {
    Belt.Option.(
      data##repository
      ->flatMap(repo => repo##pullRequest)
      ->flatMap(pr => Js.Json.decodeString(pr##createdAt))
      ->getExn
    );
  };

  let pullRequestReactionGroups = (data: Js.t('a)) => {
    Belt.Option.(
      data##repository
      ->flatMap(repo => repo##pullRequest)
      ->flatMap(pr => pr##reactionGroups)
    );
  };

  let commentAuthorLogin = (node: Js.t('a)) => {
    switch (node##author) {
    | Some(`Actor(author)) => Some(author##login)
    | _ => None
    };
  };

  let commentAuthorAvatarUrl = (node: Js.t('a)) => {
    switch (node##author) {
    | Some(`Actor(author)) => Js.Json.decodeString(author##avatarUrl)
    | _ => None
    };
  };

  let hasPreviousPage = (data: Js.t('a)) => {
    Belt.Option.(
      data##repository
      ->flatMap(repo => repo##pullRequest)
      ->mapWithDefault(false, pr => pr##comments##pageInfo##hasPreviousPage)
    );
  };

  let hasNextPage = (data: Js.t('a)) => {
    Belt.Option.(
      data##repository
      ->flatMap(repo => repo##pullRequest)
      ->mapWithDefault(false, pr => pr##comments##pageInfo##hasNextPage)
    );
  };

  let startCursor = (data: Js.t('a)) => {
    Belt.Option.(
      data##repository
      ->flatMap(repo => repo##pullRequest)
      ->flatMap(pr => pr##comments##pageInfo##startCursor)
      ->getExn
    );
  };

  let endCursor = (data: Js.t('a)) => {
    Belt.Option.(
      data##repository
      ->flatMap(repo => repo##pullRequest)
      ->flatMap(pr => pr##comments##pageInfo##endCursor)
      ->getExn
    );
  };

  let updateQuery = (prev, next) =>
    Belt.Option.getWithDefault(
      next->ReasonApolloQuery.fetchMoreResultGet,
      prev,
    );

  [@react.component]
  let make = (~owner: string, ~name: string, ~number: int) => {
    let pullRequestCommentsQuery =
      Gql_Schema.PullRequestComments.make(
        ~owner,
        ~name,
        ~number,
        ~first=100,
        (),
      );

    <Gql_Query.PullRequestComments
      variables=pullRequestCommentsQuery##variables>
      ...{({result, fetchMore}) =>
        switch (result) {
        | Loading =>
          <progress className="progress is-large is-info" max="100" />
        | Error(error) =>
          <div className="notification is-danger">
            {error##message->React.string}
          </div>
        | Data(data) =>
          <>
            <article className="media">
              <figure className="media-left">
                <p className="image is-64x64">
                  {data
                   ->pullRequestAuthorAvatarUrl
                   ->Belt.Option.mapWithDefault(
                       <img
                         src="https://bulma.io/images/placeholders/128x128.png"
                       />,
                       avatarUrl =>
                       <img src=avatarUrl />
                     )}
                </p>
              </figure>
              <div className="media-content">
                <div className="card">
                  <header className="card-header">
                    <p className="card-header-title">
                      {data
                       ->pullRequestAuthorLogin
                       ->Belt.Option.mapWithDefault(React.null, login =>
                           {j|$login started a new pull request|j}
                           ->React.string
                         )}
                    </p>
                    <p className="card-header-icon">
                      <UI_Time_Ago date={data->pullRequestCreatedAt} />
                    </p>
                  </header>
                  <div className="card-content">
                    <div className="content">
                      {data->pullRequestBodyText->React.string}
                    </div>
                  </div>
                  {data->pullRequestViewerCanReact
                     ? <footer className="card-footer">
                         {data
                          ->pullRequestReactionGroups
                          ->Belt.Option.mapWithDefault(
                              React.null, reactionGroups =>
                              <UI_Reaction_Groups
                                subjectId={data->pullRequestId}
                                reactionGroups
                              />
                            )}
                       </footer>
                     : React.null}
                </div>
              </div>
            </article>
            {data##repository
             ->Belt.Option.flatMap(repo => repo##pullRequest)
             ->Belt.Option.flatMap(pr => pr##comments##nodes)
             ->Belt.Option.mapWithDefault(
                 "no pull request comments" |> React.string, nodes =>
                 nodes->Belt.Array.keepMap(x => x)->Belt.Array.reverse
                 |> Array.map(node =>
                      <article className="media">
                        <figure className="media-left">
                          <p className="image is-64x64">
                            {node
                             ->commentAuthorAvatarUrl
                             ->Belt.Option.mapWithDefault(
                                 <img
                                   src="https://bulma.io/images/placeholders/128x128.png"
                                 />,
                                 avatarUrl =>
                                 <img src=avatarUrl />
                               )}
                          </p>
                        </figure>
                        <div className="media-content">
                          <div className="card">
                            <header className="card-header">
                              <p className="card-header-title">
                                {node
                                 ->commentAuthorLogin
                                 ->Belt.Option.mapWithDefault(
                                     React.null, login =>
                                     {j|$login left a comment|j}->React.string
                                   )}
                              </p>
                              <p className="card-header-icon">
                                <UI_Time_Ago
                                  date={
                                    Js.Json.decodeString(node##createdAt)
                                    ->Belt.Option.getExn
                                  }
                                />
                              </p>
                              {node##viewerCanDelete
                                 ? <Gql_Mutation.DeleteIssueComment>
                                     ...{(mutate, {result}) =>
                                       <a
                                         disabled={result == Loading}
                                         className="card-header-icon"
                                         onClick={_ => {
                                           let deleteIssueComment =
                                             Gql_Schema.DeleteIssueComment.make(
                                               ~id=node##id,
                                               (),
                                             );
                                           mutate(
                                             ~variables=
                                               deleteIssueComment##variables,
                                             ~refetchQueries=[|
                                               "pullRequestComments",
                                             |],
                                             (),
                                           )
                                           |> ignore;
                                         }}>
                                         <span className="icon">
                                           <i
                                             className="fas fa-trash-alt"
                                             ariaHidden=true
                                           />
                                         </span>
                                       </a>
                                     }
                                   </Gql_Mutation.DeleteIssueComment>
                                 : React.null}
                            </header>
                            <div className="card-content">
                              <div className="content">
                                {node##bodyText->React.string}
                              </div>
                            </div>
                            {node##viewerCanReact
                               ? <footer className="card-footer">
                                   {node##reactionGroups
                                    ->Belt.Option.mapWithDefault(
                                        React.null, reactionGroups =>
                                        <UI_Reaction_Groups
                                          subjectId=node##id
                                          reactionGroups
                                        />
                                      )}
                                 </footer>
                               : React.null}
                          </div>
                        </div>
                      </article>
                    )
                 |> React.array
               )}
            {data->hasPreviousPage
               ? {
                 let morePullRequestCommentsQuery =
                   Gql_Schema.PullRequestComments.make(
                     ~owner,
                     ~name,
                     ~number,
                     ~last=100,
                     ~before=data->startCursor,
                     (),
                   );
                 <UI_Control.Button
                   text="Prev Page"
                   onClick={_ =>
                     fetchMore(
                       ~variables=morePullRequestCommentsQuery##variables,
                       ~updateQuery,
                       (),
                     )
                     |> ignore
                   }
                 />;
               }
               : React.null}
            {data->hasNextPage
               ? {
                 let morePullRequestCommentsQuery =
                   Gql_Schema.PullRequestComments.make(
                     ~owner,
                     ~name,
                     ~number,
                     ~first=100,
                     ~after=data->endCursor,
                     (),
                   );
                 <UI_Control.Button
                   text="Next Page"
                   onClick={_ =>
                     fetchMore(
                       ~variables=morePullRequestCommentsQuery##variables,
                       ~updateQuery,
                       (),
                     )
                     |> ignore
                   }
                 />;
               }
               : React.null}
            <UI_Comments.New
              subjectId={data->pullRequestId}
              refetchQueries=[|"pullRequestComments"|]
            />
          </>
        }
      }
    </Gql_Query.PullRequestComments>;
  };
};