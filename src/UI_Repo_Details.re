let id = (data: Js.t('a)) => {
  Belt.Option.(data##repository->map(repo => repo##id)->getExn);
};

let name = (data: Js.t('a)) => {
  Belt.Option.(data##repository->map(repo => repo##name)->getExn);
};

let description = (data: Js.t('a)) => {
  Belt.Option.(
    data##repository->flatMap(repo => repo##description)->getWithDefault("")
  );
};

let nameWithOwner = (data: Js.t('a)) => {
  Belt.Option.(data##repository->map(repo => repo##nameWithOwner)->getExn);
};

let viewerHasStarred = (data: Js.t('a)) => {
  data##repository
  ->Belt.Option.mapWithDefault(false, repo => repo##viewerHasStarred);
};

let stargazersCount = (data: Js.t('a)) => {
  Belt.Option.(
    data##repository->mapWithDefault(0, repo => repo##stargazers##totalCount)
  );
};

let forksCount = (data: Js.t('a)) => {
  Belt.Option.(
    data##repository->mapWithDefault(0, repo => repo##forks##totalCount)
  );
};

[@react.component]
let make = (~repo, ~children) => {
  let (descriptionModalIsActive, toggleDescriptionModal) =
    React.useState(() => false);

  <div className="card is-shadowless">
    <header className="card-header">
      <div className="card-header-title">
        <nav className="breadcrumb is-medium" ariaLabel="breadcrumbs">
          Route.(
            switch (ReasonReact.Router.useUrl()->urlToRoute) {
            | Repo_Files_New(owner, name) =>
              <ul>
                <li>
                  <a
                    onClick={_ =>
                      ReasonReact.Router.push({j|/$owner/$name|j})
                    }>
                    name->React.string
                  </a>
                </li>
                <li className="is-active"> <a> "New"->React.string </a> </li>
              </ul>

            | Repo_Files_Edit(owner, name, path) =>
              <ul>
                <li>
                  <a
                    onClick={_ =>
                      ReasonReact.Router.push({j|/$owner/$name|j})
                    }>
                    name->React.string
                  </a>
                </li>
                <li>
                  <a
                    onClick={_ =>
                      ReasonReact.Router.push(
                        {j|/$owner/$name/blob/master/$path|j},
                      )
                    }>
                    {path->Js.Global.decodeURI->React.string}
                  </a>
                </li>
                <li className="is-active"> <a> "Edit"->React.string </a> </li>
              </ul>
            | Repo_Files_Delete(owner, name, path) =>
              <ul>
                <li>
                  <a
                    onClick={_ =>
                      ReasonReact.Router.push({j|/$owner/$name|j})
                    }>
                    name->React.string
                  </a>
                </li>
                <li>
                  <a
                    onClick={_ =>
                      ReasonReact.Router.push(
                        {j|/$owner/$name/blob/master/$path|j},
                      )
                    }>
                    {path->Js.Global.decodeURI->React.string}
                  </a>
                </li>
                <li className="is-active">
                  <a> "Delete"->React.string </a>
                </li>
              </ul>
            | Repo_Files_Show(owner, name, path) =>
              <ul>
                <li>
                  <a
                    onClick={_ =>
                      ReasonReact.Router.push({j|/$owner/$name|j})
                    }>
                    name->React.string
                  </a>
                </li>
                <li className="is-active">
                  <a> {path->Js.Global.decodeURI->React.string} </a>
                </li>
              </ul>
            | _ =>
              <ul>
                <li className="is-active">
                  <a> {repo->name->React.string} </a>
                </li>
              </ul>
            }
          )
        </nav>
      </div>
      <div className="card-header-icon">
        <a className="button" onClick={_ => toggleDescriptionModal(_ => true)}>
          <span className="icon">
            <i className="fas fa-book" ariaHidden=true />
          </span>
          <span> "Description"->React.string </span>
        </a>
      </div>
      <div className="card-header-icon">
        <UI_Star
          starrableId={repo->id}
          viewerHasStarred={repo->viewerHasStarred}
          numberOfStars={repo->stargazersCount}
        />
      </div>
      <div className="card-header-icon">
        <span className="icon">
          <i className="fas fa-code-branch" ariaHidden=true />
        </span>
        {repo->forksCount->string_of_int->React.string}
      </div>
    </header>
    <div className="card-content"> children </div>
    <UI_Modal
      title="Description"
      isActive=descriptionModalIsActive
      onClose={() => toggleDescriptionModal(_ => false)}>
      <p> {repo->description->React.string} </p>
    </UI_Modal>
  </div>;
};