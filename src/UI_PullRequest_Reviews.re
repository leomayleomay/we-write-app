module Index = {
  let authorLogin = (node: Js.t('a)) => {
    switch (node##author) {
    | Some(`Actor(author)) => Some(author##login)
    | _ => None
    };
  };

  let authorAvatarUrl = (node: Js.t('a)) => {
    switch (node##author) {
    | Some(`Actor(author)) =>
      Js.Json.decodeString(author##avatarUrl)->Belt.Option.getExn

    | _ => "https://bulma.io/images/placeholders/128x128.png"
    };
  };

  let hasPreviousPage = (data: Js.t('a)) => {
    Belt.Option.(
      data##repository
      ->flatMap(repo => repo##pullRequest)
      ->flatMap(pullRequest => pullRequest##reviews)
      ->mapWithDefault(false, reviews => reviews##pageInfo##hasPreviousPage)
    );
  };

  let hasNextPage = (data: Js.t('a)) => {
    Belt.Option.(
      data##repository
      ->flatMap(repo => repo##pullRequest)
      ->flatMap(pullRequest => pullRequest##reviews)
      ->mapWithDefault(false, reviews => reviews##pageInfo##hasNextPage)
    );
  };

  let startCursor = (data: Js.t('a)) => {
    Belt.Option.(
      data##repository
      ->flatMap(repo => repo##pullRequest)
      ->flatMap(pullRequest => pullRequest##reviews)
      ->flatMap(reviews => reviews##pageInfo##startCursor)
      ->getExn
    );
  };

  let endCursor = (data: Js.t('a)) => {
    Belt.Option.(
      data##repository
      ->flatMap(repo => repo##pullRequest)
      ->flatMap(pullRequest => pullRequest##reviews)
      ->flatMap(reviews => reviews##pageInfo##endCursor)
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
    let pullRequestReviewsQuery =
      Gql_Schema.PullRequestReviews.make(
        ~owner,
        ~name,
        ~number,
        ~first=100,
        (),
      );

    <Gql_Query.PullRequestReviews variables=pullRequestReviewsQuery##variables>
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
            {data##repository
             ->Belt.Option.flatMap(repo => repo##pullRequest)
             ->Belt.Option.flatMap(pr => pr##reviews)
             ->Belt.Option.flatMap(reviews => reviews##nodes)
             ->Belt.Option.mapWithDefault("no reviews" |> React.string, nodes =>
                 <ul style={ReactDOMRe.Style.make(~listStyleType="none", ())}>
                   {nodes->Belt.Array.keepMap(x => x)->Belt.Array.reverse
                    |> Array.map(node =>
                         <article className="media">
                           <figure className="media-left">
                             <p className="image is-64x64">
                               <img src={node->authorAvatarUrl} />
                             </p>
                           </figure>
                           <div className="media-content">
                             <div className="card">
                               <header className="card-header">
                                 <p className="card-header-title">
                                   {node
                                    ->authorLogin
                                    ->Belt.Option.mapWithDefault(
                                        React.null, login =>
                                        switch (node##state) {
                                        | `APPROVED =>
                                          {j|$login approved your changes|j}
                                          ->React.string
                                        | `COMMENTED =>
                                          {j|$login commented your changes|j}
                                          ->React.string
                                        | `CHANGES_REQUESTED =>
                                          {j|$login requested changes|j}
                                          ->React.string
                                        | _ => React.null
                                        }
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
                                  &&
                                  node##state == `PENDING
                                    ? <Gql_Mutation.DeletePullRequestReview>
                                        ...{(mutate, {result}) =>
                                          <a
                                            disabled={result == Loading}
                                            className="card-header-icon"
                                            onClick={_ => {
                                              let deletePullRequestReview =
                                                Gql_Schema.DeletePullRequestReview.make(
                                                  ~pullRequestReviewId=
                                                    node##id,
                                                  (),
                                                );
                                              mutate(
                                                ~variables=
                                                  deletePullRequestReview##variables,
                                                ~refetchQueries=[|
                                                  "pullRequestReviews",
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
                                      </Gql_Mutation.DeletePullRequestReview>
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
                    |> React.array}
                 </ul>
               )}
            {data->hasPreviousPage
               ? {
                 let morePullRequestReviewsQuery =
                   Gql_Schema.PullRequestReviews.make(
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
                       ~variables=morePullRequestReviewsQuery##variables,
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
                 let morePullRequestReviewsQuery =
                   Gql_Schema.PullRequestReviews.make(
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
                       ~variables=morePullRequestReviewsQuery##variables,
                       ~updateQuery,
                       (),
                     )
                     |> ignore
                   }
                 />;
               }
               : React.null}
          </>
        }
      }
    </Gql_Query.PullRequestReviews>;
  };
};

module Form = {
  let pullRequestState = (data: Js.t('a)) => {
    Belt.Option.(
      data##repository
      ->flatMap(repo => repo##pullRequest)
      ->map(pr => pr##state)
      ->getExn
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

  let viewerDidAuthor = (data: Js.t('a)) => {
    Belt.Option.(
      data##repository
      ->flatMap(repo => repo##pullRequest)
      ->map(pr => pr##viewerDidAuthor)
      ->getExn
    );
  };

  [@react.component]
  let make = (~pullRequest) => {
    let (body, setBody) = React.useState(() => "");
    let (event, setEvent) = React.useState(() => `COMMENT);
    let (isActive, toggleModal) = React.useState(() => false);

    React.useEffect1(
      () => {
        String.length(body) > 0
          ? OnBeforeUnload.register() : OnBeforeUnload.unregister();
        Some(() => OnBeforeUnload.unregister());
      },
      [|body|],
    );

    <>
      <div className="level">
        <div className="level-left" />
        <div className="level-right">
          <div className="level-item">
            <button
              className="button is-primary"
              onClick={_ => toggleModal(_ => true)}>
              <span> "Add your review"->React.string </span>
            </button>
          </div>
        </div>
      </div>
      <UI_Modal
        title="New Review" isActive onClose={() => toggleModal(_ => false)}>
        <Gql_Mutation.AddPullRequestReview>
          ...{(mutate, {result}) =>
            <>
              {switch (pullRequest->pullRequestState) {
               | `CLOSED
               | `MERGED => React.null
               | `OPEN =>
                 <div className="field">
                   <div className="control">
                     <label className="radio">
                       <input
                         type_="radio"
                         checked={event == `COMMENT}
                         onChange={_ => setEvent(_ => `COMMENT)}
                       />
                       {"Comment" |> React.string}
                     </label>
                     <label
                       className="radio"
                       disabled={pullRequest->viewerDidAuthor}>
                       <input
                         type_="radio"
                         checked={event == `APPROVE}
                         onChange={_ => setEvent(_ => `APPROVE)}
                       />
                       {"Approve" |> React.string}
                     </label>
                     <label
                       className="radio"
                       disabled={pullRequest->viewerDidAuthor}>
                       <input
                         type_="radio"
                         checked={event == `REQUEST_CHANGES}
                         onChange={_ => setEvent(_ => `REQUEST_CHANGES)}
                       />
                       {"Request Changes" |> React.string}
                     </label>
                   </div>
                 </div>
               }}
              <div className="field">
                <label className="label"> {"Body" |> React.string} </label>
                <div className="control">
                  <textarea
                    className="textarea"
                    value=body
                    rows=3
                    disabled={result == Loading}
                    onChange={e => setBody(ReactEvent.Form.target(e)##value)}
                  />
                </div>
              </div>
              <div className="field">
                <div className="control">
                  <button
                    className={
                      result == Loading
                        ? "button is-primary is-loading" : "button is-primary"
                    }
                    disabled={result == Loading}
                    onClick={_ => {
                      let addPullRequestReview =
                        Gql_Schema.AddPullRequestReview.make(
                          ~pullRequestId=pullRequest->pullRequestId,
                          ~body,
                          ~event,
                          (),
                        );
                      Js.Promise.(
                        mutate(~variables=addPullRequestReview##variables, ())
                        |> then_(_ => toggleModal(_ => false) |> resolve)
                        |> ignore
                      );
                    }}>
                    {"Submit Review" |> React.string}
                  </button>
                </div>
              </div>
            </>
          }
        </Gql_Mutation.AddPullRequestReview>
      </UI_Modal>
    </>;
  };
};

module New = {
  let viewerPermission = (data: Js.t('a)) => {
    Belt.Option.(
      data##repository
      ->flatMap(repo => repo##viewerPermission)
      ->getWithDefault(`READ)
    );
  };

  [@react.component]
  let make = (~owner, ~name, ~number) => {
    let pullRequestQuery =
      Gql_Schema.PullRequest.make(~owner, ~name, ~number, ());

    <Gql_Query.PullRequest variables=pullRequestQuery##variables>
      ...{({result}) =>
        switch (result) {
        | Loading =>
          <progress className="progress is-large is-info" max="100" />
        | Error(error) =>
          <div className="notification is-danger">
            {error##message |> React.string}
          </div>
        | Data(data) =>
          switch (data->viewerPermission) {
          | `ADMIN
          | `WRITE => <Form pullRequest=data />
          | `READ => React.null
          }
        }
      }
    </Gql_Query.PullRequest>;
  };
};