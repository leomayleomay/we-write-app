let inMemoryCache = ApolloInMemoryCache.createInMemoryCache();

let authLink =
  ApolloLinks.createContextLink(() =>
    {
      "headers": {
        "Authorization":
          "Bearer " ++ Belt.Option.getExn(Auth.GithubToken.get()),
      },
    }
  );

let httpLink =
  ApolloLinks.createHttpLink(~uri="https://api.github.com/graphql", ());

let instance =
  ReasonApollo.createApolloClient(
    ~link=ApolloLinks.from([|authLink, httpLink|]),
    ~cache=inMemoryCache,
    (),
  );

let resetStore = () => instance##resetStore();