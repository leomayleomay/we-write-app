[@react.component]
let make = (~repo: Model_Repo.t) => {
  <div
    className="card"
    key={string_of_int(repo.id)}
    onClick={_ =>
      Auth.isAuthenticated()
        ? ReasonReact.Router.push(repo.owner.login ++ "/" ++ repo.name)
        : ReasonReact.Router.push("/auth/github")
    }>
    <header className="card-header">
      <p className="card-header-title"> {repo.name |> React.string} </p>
      <p className="card-header-icon">
        <span className="icon">
          <i className="fas fa-star" ariaHidden=true />
        </span>
        {repo.stargazersCount->string_of_int->React.string}
      </p>
      <p className="card-header-icon">
        <span className="icon">
          <i className="fas fa-code-branch" ariaHidden=true />
        </span>
        {repo.forksCount->string_of_int->React.string}
      </p>
    </header>
    <div className="card-content">
      <div className="media">
        <div className="media-left">
          <figure className="image is-48x48">
            <img src={repo.owner.image} alt={repo.owner.login} />
          </figure>
        </div>
        <div className="media-content">
          <p className="title is-4"> {repo.name |> React.string} </p>
          <p className="subtitle is-6"> {repo.owner.login |> React.string} </p>
        </div>
      </div>
      <div className="content">
        {repo.description
         ->Belt.Option.getWithDefault("No description ~")
         ->React.string}
      </div>
    </div>
  </div>;
};